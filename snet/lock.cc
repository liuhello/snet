
#include "lock.h"

namespace snet
{
    ///////////////////////////////////////////////
    //Lock
    ///////////////////////////////////////////////
    Lock::Lock(LockType type)
    {
        m_type = type;
    }
    Lock::~Lock()
    {
        if(m_init)
        {
            pthread_mutex_destroy(&m_mutex);
            pthread_cond_destroy(&m_cond);
        }
        m_init = false;
    }
    /*
    * Every Lock before use it should init it.
    * Like this: 
    *           Lock l;
    *           if(l.init()==0) l.lock();
    **/
    int Lock::init()
    {
        if(m_init) return 0;
        pthread_mutexattr_t attr;
        int res = pthread_mutexattr_init(&attr);
        if(res != 0) return res;
        if(m_type == RecursiveLock)
        {
            res = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
            if(res != 0)
            {
                pthread_mutexattr_destroy(&attr);
                return res;
            }
        } 
        else if(m_type == ErrorCheckLock)
        {
            res = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
            if(res != 0)
            {
                pthread_mutexattr_destroy(&attr);
                return res;
            }
        }
        res = pthread_mutex_init(&m_mutex,&attr);
        if(res == 0)
        {
            res = pthread_cond_init(&m_cond,NULL);
        }
        pthread_mutexattr_destroy(&attr);
        m_init = true;
        return res;
    }
    int Lock::lock()
    {
        if(!m_init) return -1;
        return pthread_mutex_lock(&m_mutex);
    }
    int Lock::tryLock()
    {
        if(!m_init)return -1;
        return pthread_mutex_trylock(&m_mutex);
    }
    int Lock::unlock()
    {
        if(!m_init)return -1;
        return pthread_mutex_unlock(&m_mutex);
    }
    int Lock::wait(int time)
    {
        if(!m_init) return -1;
        if(time <= 0) 
        {
            return pthread_cond_wait(&m_cond,&m_mutex);
        }
        struct timespec ts;
        ts.tv_sec = time/1000;
        ts.tv_nsec = (time%1000)*1000000l;
        return pthread_cond_timedwait(&m_cond,&m_mutex,&ts);
    }
    int Lock::signal()
    {
        if(!m_init) return -1;
        return pthread_cond_signal(&m_cond);
    }
    int Lock::signalAll()
    {
        if(!m_init) return -1;
        return pthread_cond_broadcast(&m_cond);
    }
}
