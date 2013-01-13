
#include "config.h"

#ifndef __STREAM_H__
#define __STREAM_H__

namespace snet
{
    // A ByteArrayInputStream is a util class which read data(byte short int long) from a char array.
    // this class
    class InputStream
    {
    public:
        virtual int read(char* buf,int size) = 0;
    };
    class DataInputStream : public InputStream
    {
    public:
        virtual int readByte(sint8 &b) = 0;
        virtual int readShort(sint16& v) = 0;
        virtual int readInt(sint32& v) = 0;
        virtual int readLong(sint64& v) = 0;
    };
    
    class OutputStream
    {
    public:
        virtual int write(char* buf,int size) = 0;
    };
    
    class DataOutputStream : public OutputStream
    {
    public:
        virtual int writeByte(sint8 b)  = 0;
        virtual int writeShort(sint16 b) = 0;
        virtual int writeInt(sint32 b) = 0;
        virtual int writeLong(sint64 b) = 0;
    };
    
    class ByteArrayInputStream : public DataInputStream
    {
    public:
        ByteArrayInputStream(const char* buf=NULL,int size = 0):m_buf(buf),m_size(size),m_pos(0){}
        ~ByteArrayInputStream(){if(m_buf)delete[] m_buf;m_buf = 0;}
        int read(char *buf,int size);
        int readByte(sint8 &b);
        int readShort(sint16& v);
        int readInt(sint32& v);
        int readLong(sint64& v);
    protected:
        const char* m_buf;
        int m_size;
        int m_pos;
    };
    class ByteArrayOutputStream : public DataOutputStream
    {
    public:
        ByteArrayOutputStream(int size = 512);
        ~ByteArrayOutputStream();
        int write(char* buf,int size);
        int writeByte(sint8 b);
        int writeShort(sint16 b);
        int writeInt(sint32 b);
        int writeLong(sint64 b);
        char* byteArray(){return m_buf;}
        char* toByte();
        int len(){return m_pos;}
    protected:
        bool checkMemory(int size);
    protected:
        char* m_buf;
        int m_size;
        int m_pos;
    };
    
    class ByteBuffer : public DataOutputStream, public DataInputStream
    {
    public:
        ByteBuffer(int size);
        ~ByteBuffer();
        //read method
        int read(char *buf,int size);
        int readByte(sint8 &b);
        int readShort(sint16& v);
        int readInt(sint32& v);
        int readLong(sint64& v);
        //wirte method
        int write(char* buf,int size);
        int writeByte(sint8 b);
        int writeShort(sint16 b);
        int writeInt(sint32 b);
        int writeLong(sint64 b);
    protected:
        bool checkMemory(int size);
    protected:
        char* m_buf;
        char* m_read;
        char* m_write;
        int m_size;
    };
}

#endif

