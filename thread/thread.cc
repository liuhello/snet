
#include "thread.h"

namespace snet
{
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
    
    void Thread::start()
    {
        pthread_create(&m_tid,NULL,Thread::start_thread,this);
    }
    void Thread::join()
    {
        //if(m_tid <= 0) return;
        pthread_join(m_tid,NULL);
    }
}





