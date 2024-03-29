
#ifndef __FILE_H__
#define __FILE_H__

#include <string>

#include "stream.h"
#include "misc.h"

namespace snet
{
    class File : public InputStream,public OutputStream,private NonCopyable
    {
    public:
        File(const char* name);
        ~File();
        //test is this file is opened.
        bool isOpen(){return m_fd > 0;}
        int open(int flags,int mode = 0622);
        int write(const char* buf,int size);
        int read(char* buf,int size);
        int close();
        bool remove();
        bool exist();
        int len();
        int seek(int pos);
    protected:
        int m_fd;
        std::string m_file;
    };
}


#endif
