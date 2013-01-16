
#include "thread.h"

namespace snet
{
    ////////////////////////////////////////
    //Thread
    ////////////////////////////////////////
    Thread::Thread() 
    {
        m_status = ThreadCreate;
        //m_tid = -1;
    }
    void* Thread::start_thread(void* t)
    {
        Thread *th = reinterpret_cast<Thread*>(t);
        th->m_status = ThreadRunning;
        th->run();
        th->m_status = ThreadStop;
        return (void*)0;
    }
    
    /*
    * start the thread use pthread_create,the subclass of this class should not rewrite this method,
    * it should implement the run method to do what they want to do.
    **/
    void Thread::start()
    {
        pthread_create(&m_tid,NULL,Thread::start_thread,this);
    }
    /*
    *   wait this thread until it exit.
    **/
    void Thread::join()
    {
        //if(m_tid <= 0) return;
        pthread_join(m_tid,NULL);
    }
    ////////////////////////////////////////////////
    //RThread
    ////////////////////////////////////////////////
    RThread::RThread(Runnable *r)
    {
        m_run = r;
    }
    RThread::~RThread()
    {
        if(m_run)delete m_run;
        m_run = NULL;
    }
    void RThread::run()
    {
        if(m_run)m_run->run();
    }
}





