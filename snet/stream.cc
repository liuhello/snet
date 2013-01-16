
#include "stream.h"
#include <cstdlib>
#include <cstring>

namespace snet
{
    ////////////////////////////////////////////////
    //ByteArrayInputStream
    ////////////////////////////////////////////////
    int ByteArrayInputStream::read(char* buf,int size)
    {
        if(m_size == m_pos) return -1;
        if(size > m_size - m_pos)size = m_size - m_pos;
        memcpy(buf,m_buf+m_pos,size);
        m_pos += size;
        return size;
    }
    int ByteArrayInputStream::readByte(sint8& v)
    {
        if(m_size - m_pos < 1) return -1;
        const char *tmp = m_buf+m_pos;
        v = (*tmp)&0xff;
        m_pos += 1;
        return 1;
    }
    int ByteArrayInputStream::readShort(sint16& v)
    {
        if(m_size - m_pos < 2) return -1;
        const char *tmp = m_buf + m_pos;
        v = 0;
        v += (((*tmp)&0xff)<<8);
        v += ((*(++tmp))&0xff);
        m_pos += 2;
        return 2;
    }
    int ByteArrayInputStream::readInt(sint32& v)
    {
        if(m_size - m_pos < 4) return -1;
        const char *tmp = m_buf + m_pos;
        v = 0;
        v += (((*tmp)&0xff)<<24);
        v += (((*(++tmp))&0xff)<<16);
        v += (((*(++tmp))&0xff)<<8);
        v += ((*(++tmp))&0xff);
        m_pos += 4;
        return 4;
    }
    int ByteArrayInputStream::readLong(sint64& v)
    {
        if(m_size - m_pos < 8) return -1;
        const char *tmp = m_buf + m_pos;
        v = 0;
        v += ((((sint64)((*tmp))&0xff)<<28)<<28);
        v += ((((sint64)((*(++tmp)))&0xff)<<24)<<24);
        v += ((((sint64)((*(++tmp)))&0xff)<<20)<<20);
        v += ((((sint64)((*(++tmp)))&0xff)<<16)<<16);
        v += (((sint64)((*(++tmp)))&0xff)<<24);
        v += (((sint64)((*(++tmp)))&0xff)<<16);
        v += (((*(++tmp))&0xff)<<8);
        v += ((*(++tmp))&0xff);
        m_pos += 8;
        return 8;
    }
    
    ////////////////////////////////////////////////
    //ByteArrayOutputStream
    ////////////////////////////////////////////////
    ByteArrayOutputStream::ByteArrayOutputStream(int size)
    {
        int tsize = 1;
        while(tsize < size)
        {
            tsize = tsize<<1;
        }
        m_buf = new char[tsize];
        m_size = tsize;
        m_pos = 0;
    }
    ByteArrayOutputStream::~ByteArrayOutputStream()
    {
        if(m_buf)delete[] m_buf;
        m_buf = 0;
    }
    bool ByteArrayOutputStream::checkMemory(int size)
    {
        if(m_size - m_pos >= size) return true;
        int remain = 0;
        int tsize = m_size;
        do {
            tsize = tsize << 1;
            remain = tsize - m_pos;
        } while(remain < size);
        char *tmp = new char[tsize];
        if(!tmp) return false;
        memcpy(tmp,m_buf,m_size);
        m_size = tsize;
        delete[] m_buf;
        m_buf = tmp;
        return true;
    }
    char* ByteArrayOutputStream::toByte()
    {
        if(m_pos == 0) return NULL;
        char* tmp = new char[m_pos];
        memcpy(tmp,m_buf,m_pos);
        return tmp;
    }
    
    int ByteArrayOutputStream::write(char* buf,int size)
    {
        if(!checkMemory(size)) return -1;
        char* tmp = m_buf + m_pos;
        memcpy(tmp,buf,size);
        m_pos += size;
        return size;
    }
    
    int ByteArrayOutputStream::writeByte(sint8 b)
    {
        if(!checkMemory(1)) return -1;
        *(m_buf+m_pos) = b&0xff;
        m_pos += 1;
        return 1;
    }
    int ByteArrayOutputStream::writeShort(sint16 b)
    {
        if(!checkMemory(2)) return -1;
        char *tmp = m_buf+m_pos;
        *tmp = (b>>8)&0xff;
        *(++tmp) = (b)&0xff;
        m_pos += 2;
        return 2;
    }
    int ByteArrayOutputStream::writeInt(sint32 b)
    {
        if(!checkMemory(4)) return -1;
        char *tmp = m_buf+m_pos;
        *tmp = (b>>24)&0xff;
        *(++tmp) = (b>>16)&0xff;
        *(++tmp) = (b>>8)&0xff;
        *(++tmp) = (b)&0xff;
        m_pos += 4;
        return 4;
    }
    int ByteArrayOutputStream::writeLong(sint64 b)
    {
        if(!checkMemory(8)) return -1;
        char* tmp = m_buf+m_pos;
        *tmp = ((b>>28)>>28)&0xff;
        *(++tmp) = ((b>>24)>>24)&0xff;
        *(++tmp) = ((b>>20)>>20)&0xff;
        *(++tmp) = ((b>>16)>>16)&0xff;
        *(++tmp) = (b>>24)&0xff;
        *(++tmp) = (b>>16)&0xff;
        *(++tmp) = (b>>8)&0xff;
        *(++tmp) = (b)&0xff;
        m_pos += 8;
        return 8;
    }
    
    //////////////////////////////////////////////
    //ByteBuffer
    //////////////////////////////////////////////
    ByteBuffer::ByteBuffer(int size)
    {
        if(size <= 0) size = 512;
        int tsize = 1;
        while(tsize < size)
        {
            tsize = tsize<<1;
        }
        m_buf = new char[tsize];
        m_read = m_write = m_buf;
        m_size = tsize;
        m_mark = 0;
    }
    ByteBuffer::~ByteBuffer()
    {
        delete[] m_buf;
        m_read = m_write = m_buf = 0;
        m_size = 0;
        m_mark = 0;
    }
    /*
    *  check the available memory buffer size,if it is less than size,it alloc new memory for it.
    *  if there is enough memory buffer or new memory buffer success it return true or return false
    **/
    bool ByteBuffer::checkMemory(int size)
    {
        int use = m_write - m_buf;
        if(size < m_size - use) return true;
        //copy read pos to memory buffer begin
        if(m_read > m_buf)
        {
            use = m_write - m_read;
            memcpy(m_buf,m_read,use);
            m_read = m_buf;
            m_write = m_buf+use;
        }
        // check available memory size again
        if(size < m_size - use) return true;
        int tsize = m_size;
        while(tsize - use < size)
        {
            tsize = tsize << 1;
        }
        char* tmp = new char[tsize];
        if(!tmp) return false;
        memcpy(tmp,m_buf,m_size);
        delete m_buf;
        m_read = m_buf = tmp;
        m_write = m_buf + use;
        m_size = tsize;
        return true;
    }
    //read method
    int ByteBuffer::read(char* buf,int size)
    {
        int use = m_write - m_read;
        if(use < 0) return -1;
        if(use <= size) size = use;
        memcpy(buf,m_read,size);
        m_read += size;
        return size;
    }
    int ByteBuffer::readByte(sint8& v)
    {
        if(m_write - m_read < 1) return -1;
        v = (*m_read)&0xff;
        m_read += 1;
        return 1;
    }
    int ByteBuffer::readShort(sint16& v)
    {
        if(m_write - m_read < 2) return -1;
        v = 0;
        v += (((*m_read)&0xff)<<8);
        v += ((*(++m_read))&0xff);
        m_read++;
        return 2;
    }
    int ByteBuffer::readInt(sint32& v)
    {
        if(m_write - m_read < 4) return -1;
        v = 0;
        v += (((*m_read)&0xff)<<24);
        v += (((*(++m_read))&0xff)<<16);
        v += (((*(++m_read))&0xff)<<8);
        v += ((*(++m_read))&0xff);
        m_read ++;
        return 4;
    }
    int ByteBuffer::readLong(sint64& v)
    {
        if(m_write - m_read < 8) return -1;
        v = 0;
        v += ((((sint64)((*m_read))&0xff)<<28)<<28);
        v += ((((sint64)((*(++m_read)))&0xff)<<24)<<24);
        v += ((((sint64)((*(++m_read)))&0xff)<<20)<<20);
        v += ((((sint64)((*(++m_read)))&0xff)<<16)<<16);
        v += (((sint64)((*(++m_read)))&0xff)<<24);
        v += (((sint64)((*(++m_read)))&0xff)<<16);
        v += (((*(++m_read))&0xff)<<8);
        v += ((*(++m_read))&0xff);
        m_read += 1;
        return 8;
    }
    //write method
    int ByteBuffer::write(char* buf,int size)
    {
        if(!checkMemory(size)) return -1;
        memcpy(m_write,buf,size);
        m_write += size;
        return size;
    }
    
    int ByteBuffer::writeByte(sint8 b)
    {
        if(!checkMemory(1)) return -1;
        *m_write = b&0xff;
        m_write += 1;
        return 1;
    }
    int ByteBuffer::writeShort(sint16 b)
    {
        if(!checkMemory(2)) return -1;
        *m_write = (b>>8)&0xff;
        *(++m_write) = (b)&0xff;
        m_write++;
        return 2;
    }
    int ByteBuffer::writeInt(sint32 b)
    {
        if(!checkMemory(4)) return -1;
        *m_write = (b>>24)&0xff;
        *(++m_write) = (b>>16)&0xff;
        *(++m_write) = (b>>8)&0xff;
        *(++m_write) = (b)&0xff;
        m_write++;
        return 4;
    }
    int ByteBuffer::writeLong(sint64 b)
    {
        if(!checkMemory(8)) return -1;
        *m_write = ((b>>28)>>28)&0xff;
        *(++m_write) = ((b>>24)>>24)&0xff;
        *(++m_write) = ((b>>20)>>20)&0xff;
        *(++m_write) = ((b>>16)>>16)&0xff;
        *(++m_write) = (b>>24)&0xff;
        *(++m_write) = (b>>16)&0xff;
        *(++m_write) = (b>>8)&0xff;
        *(++m_write) = (b)&0xff;
        m_write++;
        return 8;
    }
    
    void ByteBuffer::mark()
    {
        m_mark = m_read;
    }
    bool ByteBuffer::unmark()
    {
        if(m_mark == 0||m_mark < m_read) return false;
        m_read = m_mark;
        m_mark = 0;
        return true;
    }
    bool ByteBuffer::consume(int size)
    {
        if(size > len()) return false;
        m_read += size;
        return true;
    }
    
    
}

