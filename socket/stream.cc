
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
        return m_pos;
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
        return m_pos;
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
}

