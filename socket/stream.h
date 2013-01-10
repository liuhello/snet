
#include "config.h"

namespace snet
{
    // A ByteArrayInputStream is a util class which read data(byte short int long) from a char array.
    // this class 
    class ByteArrayInputStream
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
    class ByteArrayOutputStream
    {
    public:
        ByteArrayOutputStream(int size = 512);
        ~ByteArrayOutputStream();
        int writeByte(sint8 b);
        int writeShort(sint16 b);
        int writeInt(sint32 b);
        int writeLong(sint64 b);
        char* toByte();
        int len(){return m_pos;}
    protected:
        bool checkMemory(int size);
    protected:
        char* m_buf;
        int m_size;
        int m_pos;
    };
}

