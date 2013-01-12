
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#include "socket.h"

#ifndef __EVENT_H__
#define __EVENT_H__
namespace snet
{
    
    struct IOEvent
    {
        bool m_read;
        bool m_write;
        bool m_error;
    };
    
    class Event
    {
    public:
        virtual bool isRead(){return m_status&0x1;}
        virtual bool isWrite(){return m_status&0x2;}
        virtual bool hasError(){return m_status&0x4;}
        virtual void setRead(bool v){if(v)m_status |= 1;else m_status &= 0xfe;}
        virtual void setWrite(bool v){if(v)m_status |= 2;else m_status &= 0xfd;}
        virtual void setError(bool v){if(v)m_status |= 4;else m_status &= 0xfb;}
        virtual int getFileDesc() = 0;
        virtual void clear(){m_status = 0;}
    protected:
        unsigned char m_status;
    };
    
    class EventManager
    {
    public:
        virtual bool addEvent(Event* e,bool read,bool write) = 0;
        virtual bool updateEvent(Event* e,bool read,bool write) = 0;
        virtual bool removeEvent(Event* e) = 0;
        virtual int getEvent(int timeout,Event** e,int cnt) = 0;
    };
    
    class EpollEventManager : public EventManager
    {
    public:
        EpollEventManager();
        ~EpollEventManager();
        bool init();
        virtual bool addEvent(Event* e,bool read,bool write);
        virtual bool updateEvent(Event* e,bool read,bool write);
        virtual bool removeEvent(Event* e);
        virtual int getEvent(int timeout,Event** e,int cnt);
    protected:
        int m_epollFd;
    };

    class SocketEvent : public Event
    {
    public:
        SocketEvent(Socket* s);
        ~SocketEvent();
        Socket* getSocket(){return m_socket;}
        virtual int getFileDesc(){return m_socket->getFd();}
    protected:
        Socket* m_socket;
    };
}
#endif
