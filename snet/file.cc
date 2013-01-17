
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
    /*
    * open the file and return the file descriptor.
    * on failed return a negtive integer and the errno is set by the sys.
    **/
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
    /*
    *   write data of buf to the file
    *   On success, the number of bytes written is returned (zero indicates nothing was written).  
    *   On error, -1 is returned, and errno is set appropriately.
    **/
    int File::write(const char* buf,int size)
    {
        return ::write(m_fd,buf,size);
    }
    /*
    *   read data from the file.
    *   On success, the number of bytes read is returned (zero indicates end of file), and the file position is advanced by this number.  
    *   It is not an error if this number is smaller than the number of bytes requested; 
    *   this may happen for example because fewer bytes are actually available right  now  (maybe  because  we were  close  to  end-of-file,  
    *   or  because  we are reading from a pipe, or from a terminal), or because read() was interrupted by a signal.  
    *   On error, -1 is returned, and errno is set appropriately.  In this case it is left unspecified whether the file position (if any) changes.
    **/
    int File::read(char* buf,int size)
    {
        return ::read(m_fd,buf,size);
    }
    /*
    *   close this file.
    *   returns zero on success.  On error, -1 is returned, and errno is set appropriately.
    **/
    int File::close()
    {
        if(m_fd <= 0) return 0;
        int res = ::close(m_fd);
        m_fd = -1;
        return res;
    }
    bool File::remove()
    {
        close();
        if(!exist()) return true;
        return ::unlink(m_file.c_str()) == 0;
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
