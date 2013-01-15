
#include "event.h"

#ifndef WINDOWS_OS
#include <sys/epoll.h>
#endif
#include <sys/select.h>
#include <unistd.h>
#include <cstdio>

#define MAX_EPOLL_EVENTS        16

namespace snet
{
    ////////////////////////////////////////////
    //EpollEventManager
    ////////////////////////////////////////////
    SocketEventManager::SocketEventManager() 
    {
    #ifndef WINDOWS_OS
        m_epollFd = -1;
    #endif
    }
    SocketEventManager::~SocketEventManager()
    {
    #ifdef WINDOWS_OS
        std::map<int,EventStruct*>::iterator iter = m_events.begin();
        while(iter != m_events.end())
        {
            EventStruct* e = iter->second;
            delete e;
            iter++;
        }
        m_events.clear();
    #else
        if(m_epollFd > 0)close(m_epollFd);
        m_epollFd = -1;
    #endif
    }
    bool SocketEventManager::init() 
    {
    #ifndef WINDOWS_OS
        if(m_epollFd > 0)return true;
        m_epollFd = epoll_create(16);
        return m_epollFd > 0;
    #endif
        return true;
    }
    
    bool SocketEventManager::addEvent(Event* e,bool read,bool write)
    {
    #ifdef WINDOWS_OS
        if(m_events.find(e->getFileDesc()) != m_events.end())return false;
        EventStruct* es = new EventStruct;
        es->m_event = e;
        es->m_read = read;
        es->m_write = write;
        m_events.insert(std::pair<int,EventStruct*>(e->getFileDesc(),es));
        return true;
    #else
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.events = 0;
        if(read) ev.events |= EPOLLIN;
        if(write) ev.events |= EPOLLOUT;
        ev.data.fd = e->getFileDesc();
        ev.data.ptr = e;
        return epoll_ctl(m_epollFd, EPOLL_CTL_ADD, e->getFileDesc(), &ev) != -1;
    #endif
    }
    bool SocketEventManager::updateEvent(Event* e,bool read,bool write)
    {
    #ifdef WINDOWS_OS
        std::map<int,EventStruct*>::iterator iter = m_events.find(e->getFileDesc());
        if(iter == m_events.end()) return false;
        iter->second->m_read = read;
        iter->second->m_write = write;
        return true;
    #else
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.events = 0;
        if(read) ev.events |= EPOLLIN;
        if(write) ev.events |= EPOLLOUT;
        ev.data.fd = e->getFileDesc();
        ev.data.ptr = e;
        return epoll_ctl(m_epollFd, EPOLL_CTL_MOD, e->getFileDesc(), &ev) != -1;
    #endif
    }
    bool SocketEventManager::removeEvent(Event* e)
    {
    #ifdef WINDOWS_OS
        std::map<int,EventStruct*>::iterator iter = m_events.find(e->getFileDesc());
        if(iter == m_events.end()) return true;
        m_events.erase(iter);
        delete iter->second;
        return true;
    #else
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.events = 0;
        ev.data.fd = e->getFileDesc();
        ev.data.ptr = e;
        return epoll_ctl(m_epollFd, EPOLL_CTL_DEL, e->getFileDesc(), &ev) != -1;
    #endif
    }
    /*
    * Get Event from epoll use epoll_wait
    * timeout : wait time before event achieve
    * e       : Event array which to collect achieved Event
    * cnt     : size of the Event array
    * return    0 if timeout less than 0 if some error occured of size of the achieved event.
    **/
    int SocketEventManager::getEvent(int timeout,Event** e,int cnt)
    {
    #ifdef WINDOWS_OS
        fd_set rset,wset,eset;
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        FD_ZERO(&eset);
        int maxFd = 0;
        std::map<int,EventStruct*>::iterator iter = m_events.begin();
        while(iter != m_events.end())
        {
            EventStruct *es = iter->second;
            int fd = es->m_event->getFileDesc();
            if(es->m_read)FD_SET(fd,&rset);
            if(es->m_write)FD_SET(fd,&wset);
            FD_SET(fd,&eset);
            if(maxFd < fd) maxFd = fd;
            iter++;
        }
        //printf("Event size : %d maxFd : %d\n",m_events.size(),maxFd);
        struct timeval tval,*tptr = NULL;
        if(timeout > 0) 
        {
            tval.tv_sec = timeout/1000;
            tval.tv_usec = timeout%1000;
            tptr = &tval;
        }
        int res = select(maxFd+1,&rset,&wset,&eset,tptr);
        if(res <= 0) return res;
        iter = m_events.begin();
        int index = 0;
        while(iter != m_events.end())
        {
            Event* tmp = iter->second->m_event;
            tmp->clear();
            int fd = tmp->getFileDesc();
            bool add = false;
            if(FD_ISSET(fd,&rset))
            {
                tmp->setRead(true);
                add = true;
            }
            if(FD_ISSET(fd,&wset))
            {
                tmp->setWrite(true);
                add = true;
            }
            if(FD_ISSET(fd,&eset))
            {
                tmp->setError(true);
                add = true;
            }
            if(add)
            {
                e[index] = tmp;
                index ++;
            }
            if(index >= cnt) break;
            iter++;
        }
        return index;
    #else
        struct epoll_event events[MAX_EPOLL_EVENTS];
        if(cnt > MAX_EPOLL_EVENTS) cnt = MAX_EPOLL_EVENTS;
        int res = epoll_wait(m_epollFd,events,cnt,timeout);
        for(int i = 0;i < res;i++)
        {
            e[i] = (Event*)events[i].data.ptr;
            e[i]->clear();
            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                e[i]->setError(true);
            }
            if ((events[i].events & EPOLLIN) != 0) {
                e[i]->setRead(true);
            }
            if ((events[i].events & EPOLLOUT) != 0) {
                e[i]->setWrite(true);
            }
        }
        return res;
    #endif
    }
    ////////////////////////////////////////////////////////
    //SocketEvent
    ////////////////////////////////////////////////////////
    SocketEvent::SocketEvent(Socket* s)
    {
        m_socket = s;
    }
    SocketEvent::~SocketEvent()
    {
        if(m_socket) delete m_socket;
        m_socket = NULL;
    }
}
