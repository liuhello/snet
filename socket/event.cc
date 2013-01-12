
#include "event.h"

#include <sys/epoll.h>

#define MAX_EPOLL_EVENTS        16

namespace snet
{
    ////////////////////////////////////////////
    //EpollEventManager
    ////////////////////////////////////////////
    EpollEventManager::EpollEventManager() 
    {
        m_epollFd = -1;
    }
    EpollEventManager::~EpollEventManager()
    {
        if(m_epollFd > 0)close(m_epollFd);
        m_epollFd = -1;
    }
    bool EpollEventManager::init() 
    {
        if(m_epollFd > 0)return true;
        m_epollFd = epoll_create(16);
        return m_epollFd > 0;
    }
    
    bool EpollEventManager::addEvent(Event* e)
    {
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.events = 0;
        if(e->isRead()) ev.events |= EPOLLIN;
        if(e->isWrite()) ev.events |= EPOLLOUT;
        ev.data.fd = e->getFileDesc();
        ev.data.ptr = e;
        return epoll_ctl(m_epollFd, EPOLL_CTL_ADD, e->getFileDesc(), &ev) != -1;
    }
    bool EpollEventManager::updateEvent(Event* e)
    {
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.events = 0;
        if(e->isRead()) ev.events |= EPOLLIN;
        if(e->isWrite()) ev.events |= EPOLLOUT;
        ev.data.fd = e->getFileDesc();
        ev.data.ptr = e;
        return epoll_ctl(m_epollFd, EPOLL_CTL_MOD, e->getFileDesc(), &ev) != -1;
    }
    bool EpollEventManager::removeEvent(Event* e)
    {
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.events = 0;
        ev.data.fd = e->getFileDesc();
        ev.data.ptr = e;
        return epoll_ctl(m_epollFd, EPOLL_CTL_DEL, e->getFileDesc(), &ev) != -1;
    }
    /*
    * Get Event from epoll use epoll_wait
    * timeout : wait time before event achieve
    * e       : Event array which to collect achieved Event
    * cnt     : size of the Event array
    * return    0 if timeout less than 0 if some error occured of size of the achieved event.
    **/
    int EpollEventManager::getEvent(int timeout,Event** e,int cnt)
    {
        struct epoll_event events[MAX_EPOLL_EVENTS];
        if(cnt > MAX_EPOLL_EVENTS) cnt = MAX_EPOLL_EVENTS;
        int res = epoll_wait(m_epollFd,events,cnt,timeout);
        for(int i = 0;i < res;i++)
        {
            e[i] = events[i].data.ptr;
            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                e[i]->setError(true);
            }
            if ((events[i].events & EPOLLIN) != 0) {
                ioevents[i]->setRead(true);
            }
            if ((events[i].events & EPOLLOUT) != 0) {
                ioevents[i]->setWrite(true);
            }
        }
        return res;
    }
}
