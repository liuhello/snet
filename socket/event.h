
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
        virtual void handleRead() = 0;
        virtual void handleWrite() = 0;
        virtual bool isRead(){return false;}
        virtual bool isWrite(){return false;}
        virtual bool hasError(){return false;}
        virtual void setRead(bool v){}
        vritual void setWrite(bool v){}
        virtual void setError(bool v){}
        virtual int getFileDesc() = 0;
    };
    
    class EventManager
    {
    public:
        virtual bool addEvent(Event* e) = 0;
        virtual bool updateEvent(Event* e) = 0;
        virtual bool removeEvent(Event* e) = 0;
        virtual int getEvent(int timeout,Event* e,int cnt) = 0;
    };
    
    class EpollEventManager : public EventManager
    {
    public:
        EpollEventManager();
        ~EpollEventManager();
        bool init();
        virtual bool addEvent(Event* e);
        virtual bool updateEvent(Event* e);
        virtual bool removeEvent(Event* e);
        virtual int getEvent(int timeout,Event* e,int cnt);
    protected:
        int m_epollFd;
    };
}
#endif
