
#include "event.h"
#include "stream.h"

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

namespace snet
{
    class Connection : public SocketEvent
    {
    public:
        Connection(Socket* s);
        ~Connection();
        bool handleRead();
        bool handleWrite();
        ByteBuffer* getReadBuffer(){return m_read;}
        ByteBuffer* getWriteBuffer(){return m_write;}
        bool isServerSocket(){return false;}
    protected:
        ByteBuffer* m_read;
        ByteBuffer* m_write;
    };
    
    class ServerSocketEvent : public SocketEvent
    {
    public:
        ServerSocketEvent(ServerSocket *ss,EventManager* manager):SocketEvent(ss),m_manager(manager){}
        ~ServerSocketEvent();
        bool handleRead();
        bool handleWrite();
        bool isServerSocket(){return true;}
    protected:
        EventManager* m_manager;
    };
    
}



#endif