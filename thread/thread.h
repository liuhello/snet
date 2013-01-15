
#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>

namespace snet
{
    enum
    {
        ThreadCreate = 0,
        ThreadRunning,
        ThreadStop
    };
    class Thread
    {
    public:
        Thread();
        virtual void run() = 0;
        virtual void start();
        virtual void join();
        int getStatus(){return m_status;}
    private:
        static void* start_thread(void *t);
    protected:
        pthread_t m_tid;
        int m_status;
    };
    
    class Runnable
    {
    public:
        virtual void run()=0;
    };
    
    class RThread : public Thread
    {
    public:
        RThread(Runnable *r);
        ~RThread();
        void run();
    protected:
        Runnable* m_run;
    };
    
    
}
#endif

