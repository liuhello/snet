
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"

namespace snet
{
    /////////////////////////////////////////////////
    //File
    /////////////////////////////////////////////////
    File::File(const char* file)
    {
        m_file = file;
        m_fd = -1;
    }
    File::~File()
    {
        m_file.clear();
        if(m_fd > 0)
        {
            close();
        }
    }
    
    int File::open(int flags,int mode)
    {
        if(m_fd > 0)
        {
            int res = close();
            if(res < 0) return res;
        }
        m_fd = ::open(m_file.c_str(),flags,mode);
        return m_fd;
    }
    int File::write(const char* buf,int size)
    {
        return ::write(m_fd,buf,size);
    }
    int File::read(char* buf,int size)
    {
        return ::read(m_fd,buf,size);
    }
    int File::close()
    {
        if(m_fd <= 0) return 0;
        int res = ::close(m_fd);
        m_fd = -1;
        return res;
    }
    int File::remove()
    {
        close();
        return ::unlink(m_file.c_str());
    }
    bool File::exist()
    {
        return ::access(m_file.c_str(),F_OK) == 0;
    }
    int File::len()
    {
        struct stat st;
        int res = lstat(m_file.c_str(),&st);
        if(res < 0) return res;
        return st.st_size;
    }
    int File::seek(int pos)
    {
        return ::lseek(m_fd,pos,SEEK_SET);
    }
    
}
