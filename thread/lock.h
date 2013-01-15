
#ifndef __LOCK_H__
#define __LOCK_H__

#include <pthread.h>
#include <time.h>

namespace snet
{
    enum LockType
    {
        NormalLock = 0,
        RecursiveLock,
        ErrorCheckLock
    };

    class Cond;
    class Lock
    {
    public:
        Lock(LockType type = NormalLock);
        ~Lock();
        int init();
        int lock();
        int tryLock();
        int unlock();
        int wait(int time = 0);
        int signal();
        int signalAll();
        friend class Connd;
    protected:
        pthread_mutex_t m_mutex;
        pthread_cond_t  m_cond;
        LockType m_type;
        bool m_init;
    };
    
    /*
    class RWLock
    {
    public:
        RWLock(LockType type);
        ~RWLock();
        int rlock();
        int wlock();
        
    };*/
    
    
}


#endif
