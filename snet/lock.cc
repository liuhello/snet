
#include "lock.h"

namespace snet
{
    ///////////////////////////////////////////////
    //Lock
    ///////////////////////////////////////////////
    Lock::Lock(LockType type)
    {
        m_type = type;
        m_init = false;
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
    /////////////////////////////////////////////////////
    //LockGuard
    /////////////////////////////////////////////////////
    LockGuard::LockGuard(Lock* lock)
    {
        m_lock = lock;
        if(m_lock)m_lock->lock();
    }
    LockGuard::~LockGuard()
    {
        if(m_lock)m_lock->unlock();
    }
    
    /////////////////////////////////////////////////////
    //RWLock
    //////////////////////////////////////////////////////
    RWLock::RWLock(RWLockType type)
    {
        m_type = type;
        m_init = false;
    }
    RWLock::~RWLock()
    {
        if(m_init)
        {
            pthread_rwlock_destroy(&m_lock);
        }
        m_init = false;
    }
    int RWLock::init()
    {
        if(m_init) return 0;
        int res = -1;
        pthread_rwlockattr_t attr;
        res = pthread_rwlockattr_init(&attr);
        if(res != 0)return res;
        if(m_type == SharedRWLock)
        {
            res = pthread_rwlockattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
        }
        if(res != 0)return res;
        res = pthread_rwlock_init(&m_lock,&attr);
        if(res != 0) return res;
        pthread_rwlockattr_destroy(&attr);
        m_init = true;
        return res;
    }
    int RWLock::rdlock()
    {
        if(!m_init)return -1;
        return pthread_rwlock_rdlock(&m_lock);
    }
    int RWLock::tryRdlock()
    {
        if(!m_init) return -1;
        return pthread_rwlock_tryrdlock(&m_lock);
    }
    int RWLock::wrlock()
    {
        if(!m_init)return -1;
        return pthread_rwlock_wrlock(&m_lock);
    }
    int RWLock::tryWrlock()
    {
        if(!m_init)return -1;
        return pthread_rwlock_trywrlock(&m_lock);
    }
    int RWLock::unlock()
    {
        if(!m_init)return -1;
        return pthread_rwlock_unlock(&m_lock);
    }
    
    /////////////////////////////////////////////
    //RWLockRDGuard
    /////////////////////////////////////////////
    RWLockRDGuard::RWLockRDGuard(RWLock*lock)
    {
        m_lock = lock;
        if(m_lock)m_lock->rdlock();
    }
    RWLockRDGuard::~RWLockRDGuard()
    {
        if(m_lock)m_lock->unlock();
    }
    ///////////////////////////////////////////////
    //RWLockWRGuard
    ///////////////////////////////////////////////
    RWLockWRGuard::RWLockWRGuard(RWLock *lock)
    {
        m_lock = lock;
        if(m_lock)m_lock->wrlock();
    }
    RWLockWRGuard::~RWLockWRGuard()
    {
        if(m_lock)m_lock->unlock();
    }
}
