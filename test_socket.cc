
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include <iostream>

#include "socket/socket.h"
#include "socket/packet.h"
#include "socket/stream.h"
#include "config.h"

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
    while(true)
    {
        Socket* s = ss->accept();
        if(s)
        {
            printf("accept a socket from : %s\n",s->getAddress().c_str());
            s->setTcpNoDelay(true);
            while(!s->isClosed())
            {
                char buf[1024];
                int t = s->read(buf,1024);
                if(t <= 0) 
                {
                    s->close();
                    break;
                }
                //s->write(buf,t);
                assert(writeFull(buf,t,s) == t);
                if(t < 0)s->close();
            }
            printf("close socket from : %s\n",s->getAddress().c_str());
            delete s;
        }
    }
}

sint64 _time = 1;

void testSocket()
{
    Socket s;
    assert(s.setAddress("127.0.0.1",8888));
    s.connect();
    printf("connect to : 127.0.0.1:8888 success......\n");
    s.setTcpNoDelay(true);
    PingPacket pp;
    pp.setTime(_time);
    ByteArrayOutputStream os;
    pp.write(&os);
    char *buf = os.toByte();
    assert(writeInt(os.len(),&s) == 4);
    assert(writeFull(buf,os.len(),&s) == os.len());
    printf("test write success.....\n");
    delete[] buf;
    
    int len = 0;
    assert(readInt(len,&s)==4);
    assert(len == os.len());
    buf = new char[len];
    assert(readFull(buf,len,&s) == len);
    ByteArrayInputStream is(buf,len);
    PingPacket pp2;
    assert(pp2.read(&is));
    delete[] buf;
    s.close();
    assert(pp2.getTime() == pp.getTime());
    printf("test read success.....\n");
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
    }
    else
    {
        for(int i = 0;i < 1000;i++)
        {
            testSocket();
        }
    }
    exit(0);
    return 0;
}
