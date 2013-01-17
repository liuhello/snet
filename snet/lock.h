
#ifndef __LOCK_H__
#define __LOCK_H__

#include <pthread.h>
#include <time.h>

#include "misc.h"

namespace snet
{
    enum LockType
    {
        NormalLock = 0,
        RecursiveLock,
        ErrorCheckLock
    };
    enum RWLockType
    {
        SharedRWLock = 0,
        PrivateRWLock
    };

    class Lock : private NonCopyable
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
    protected:
        pthread_mutex_t m_mutex;
        pthread_cond_t  m_cond;
        LockType m_type;
        bool m_init;
    };
    
    class LockGuard
    {
    public:
        LockGuard(Lock* lock);
        ~LockGuard();
    protected:
        Lock* m_lock;
    };
    
    class RWLock : private NonCopyable
    {
    public:
        RWLock(RWLockType type);
        ~RWLock();
        int init();
        int rdlock();
        int tryRdlock();
        int wrlock();
        int tryWrlock();
        int unlock();
    protected:
        RWLockType m_type;
        pthread_rwlock_t m_lock;
        bool m_init;
    };
    
    class RWLockRDGuard
    {
    public:
        RWLockRDGuard(RWLock *lock);
        ~RWLockRDGuard();
    protected:
        RWLock *m_lock;
    };
    class RWLockWRGuard
    {
    public:
        RWLockWRGuard(RWLock *lock);
        ~RWLockWRGuard();
    protected:
        RWLock *m_lock;
    };
    
    
}


#endif
