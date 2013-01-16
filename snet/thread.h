
#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <unistd.h>

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
        static int sleep(unsigned int seconds){return ::sleep(seconds);}
        static int msleep(unsigned int milliseconds){return ::usleep(milliseconds*1000l);}
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

