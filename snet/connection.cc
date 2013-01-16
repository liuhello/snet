
#include "connection.h"
#include <cstdio>

namespace snet
{
    #define MAX_BUF_SIZE        1024*4
    /////////////////////////////////////////////
    //Connection
    /////////////////////////////////////////////
    Connection::Connection(Socket* s):SocketEvent(s)
    {
        m_read = new ByteBuffer(512);
        m_write = new ByteBuffer(512);
    }
    Connection::~Connection()
    {
        if(m_read)delete m_read;
        m_read = NULL;
        if(m_write)delete m_write;
        m_write = NULL;
    }
    bool Connection::handleRead()
    {
        
        char buf[MAX_BUF_SIZE];
        int res = m_socket->read(buf,MAX_BUF_SIZE);
        if(res <= 0) return false;
        //printf("read %d byte from connection\n",res);
        return m_read->write(buf,res) == res;
    }
    bool Connection::handleWrite()
    {
        int res = m_socket->write(m_write->array(),m_write->len());
        if(res < 0) return false;
        m_write->consume(res);
        return true;
    }
    
    /////////////////////////////////////////////
    //ServerSocketEvent
    /////////////////////////////////////////////
    ServerSocketEvent::~ServerSocketEvent()
    {
        m_manager = NULL;
    }
    
    bool ServerSocketEvent::handleRead()
    {
        if(!isRead()) return true;
        ServerSocket* ss = (ServerSocket*)m_socket;
        Socket *s = ss->accept();
        if(s)
        {
            //printf("accept socket : %s\n",s->getAddress().c_str());
            s->setTcpNoDelay(true);
            Connection *conn = new Connection(s);
            return m_manager->addEvent(conn,true,false);
        }
        return true;
    }
    bool ServerSocketEvent::handleWrite()
    {
        return true;
    }
}
