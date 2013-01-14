
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include "socket/socket.h"
#include "socket/packet.h"
#include "socket/stream.h"
#include "config.h"
#include "socket/event.h"
#include "socket/connection.h"
#include "thread/thread.h"

using namespace snet;

int readFull(char* buf,int size,Socket* s)
{
    int remain = size;
    char* tmp = buf;
    while(remain > 0)
    {
        int t = s->read(tmp,remain);
        if(t < 0)return t;
        remain -= t;
        tmp = tmp+t;
    }
    return size;
}

int readInt(int& v,Socket* s)
{
    char buf[4];
    char* tmp = buf;
    int t = readFull(tmp,4,s);
    if(t < 0)return -t;
    v = 0;
    v += (((*tmp)&0xff)<<24);
    v += (((*(++tmp))&0xff)<<16);
    v += (((*(++tmp))&0xff)<<8);
    v += ((*(++tmp))&0xff);
    return 4;
}

int writeFull(char* buf,int size,Socket* s)
{
    int remain = size;
    char *tmp = buf;
    while(remain > 0)
    {
        int t = s->write(tmp,remain);
        if(t < 0) return t;
        remain -= t;
        tmp = tmp + t;
    }
    return size;
}

int writeInt(int b,Socket* s)
{
    char buf[4];
    char* tmp = buf;
    *tmp = (b>>24)&0xff;
    *(++tmp) = (b>>16)&0xff;
    *(++tmp) = (b>>8)&0xff;
    *(++tmp) = (b)&0xff;
    return writeFull(buf,4,s);
}

void echoServer(ServerSocket *ss)
{
    SocketEventManager eem;
    eem.init();
    ServerSocketEvent sse(ss,&eem);
    if(!eem.addEvent(&sse,true,false))printf("error occur errno : %d str : %s fd : %d\n",errno,strerror(errno),ss->getFd());
    Event **se = new Event*[12];
    while(true)
    {
        int count = eem.getEvent(0,se,12);
        //printf("get %d event from event manager.\n",count);
        if(count < 0) {printf("count : %d error occur errno : %d str : %s fd : %d line : %d file : %s\n",count,errno,strerror(errno),ss->getFd(),__LINE__,__FILE__);};
        assert(count >= 0);
        for(int i = 0;i < count;i++)
        {
            SocketEvent *septr = (SocketEvent*)se[i];
            if(septr->isWrite())
            {
                if(!septr->handleWrite())
                {
                    eem.removeEvent(septr);
                    printf("socket : %s handle write failed close it.\n",septr->getSocket()->getAddress().c_str());
                    delete septr;
                    continue;
                }
                if(!septr->isServerSocket())
                {
                    Connection* conn = (Connection*)septr;
                    ByteBuffer *bb = conn->getWriteBuffer();
                    if(bb->len() <= 0)
                    {
                        eem.updateEvent(conn,true,false);
                    }
                }
            }
            if(septr->isRead())
            {
                if(!septr->handleRead())
                {
                    eem.removeEvent(septr);
                    printf("socket : %s hanlde read failed close it.\n",septr->getSocket()->getAddress().c_str());
                    delete septr;
                    continue;
                }
            }
            if(septr->hasError())
            {
                eem.removeEvent(septr);
                printf("socket : %s hasError close it.\n",septr->getSocket()->getAddress().c_str());
                printf("errno : %d error str: %s\n",errno,strerror(errno));
                delete septr;
                continue;
            }
            if(!septr->isServerSocket())
            {
                Connection* conn = (Connection*)septr;
                ByteBuffer *bb = conn->getReadBuffer();
                if(bb->len() > 0)
                {
                    int len = bb->len();
                    char* tmp = new char[len];
                    assert(bb->read(tmp,len) == len);
                    bb = conn->getWriteBuffer();
                    assert(bb->write(tmp,len) == len);
                    eem.updateEvent(conn,true,true);
                }
            }
        }
    }
}

sint64 _time = 11111111111l;

void __testSocket()
{
    Socket s;
    assert(s.setAddress("127.0.0.1",8888));
    assert(s.connect());
    //printf("connect to : 127.0.0.1:8888 success......\n");
    s.setTcpNoDelay(true);
    PingPacket pp;
    pp.setTime(_time);
    ByteArrayOutputStream os;
    pp.write(&os);
    char *buf = os.toByte();
    assert(writeInt(os.len(),&s) == 4);
    assert(writeFull(buf,os.len(),&s) == os.len());
    //printf("test write success.....\n");
    delete[] buf;
    
    int len = 0;
    assert(readInt(len,&s)==4);
    assert(len == os.len());
    buf = new char[len];
    assert(readFull(buf,len,&s) == len);
    ByteArrayInputStream is(buf,len);
    PingPacket pp2;
    assert(pp2.read(&is));
    s.close();
    assert(pp2.getTime() == pp.getTime());
    //printf("test read success.....\n");
}

class TestThread : public Thread
{
public:
    void run()
    {
        for(int i = 0;i < 10;i++)
        {
            __testSocket();
        }
    }
};

void testSocket()
{
    const int num = 10;
    TestThread th[num];
    for(int i = 0;i < num;i++)
    {
        th[i].start();
    }
    for(int i = 0;i < num;i++)
    {
        th[i].join();
    }
    
}

int main(int argc,char** argv)
{
    
    ServerSocket ss(16);
    assert(ss.setAddress("127.0.0.1",8888));
    assert(ss.listen());
    
    int pid = fork();
    if(pid == 0) 
    {
        echoServer(&ss);
        ss.close();
    }
    else
    {
        _time += 1000000;
        testSocket();
        kill(pid,SIGABRT);
    }
    return 0;
}
