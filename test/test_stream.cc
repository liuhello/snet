
#include "snet/stream.h"
#include "snet/packet.h"
#include "snet/config.h"
#include <cstdlib>
#include <cstdio>
#include <assert.h>
#include <vector>

int main(int argc,char** argv)
{
    snet::ByteArrayOutputStream os;
    os.writeByte(1);
    os.writeShort(2);
    os.writeInt(3);
    os.writeLong(4);
    os.writeLong(11111111114l);
    sint8 i8;
    sint16 i16;
    sint32 i32;
    sint64 i64 = 11111111114l;
    snet::PingPacket pp(i64);
    pp.write(&os);
    
    snet::ByteArrayInputStream is(os.toByte(),os.len());
    is.readByte(i8);
    assert(i8 == 1);
    is.readShort(i16);
    assert(i16 == 2);
    is.readInt(i32);
    assert(i32 == 3);
    is.readLong(i64);
    assert(i64 == 4);
    is.readLong(i64);
    assert(i64 == 11111111114l);
    snet::PingPacket pp2;
    pp2.read(&is);
    assert(pp.getTime() == pp2.getTime());
    
    snet::ByteBuffer bb(16);
    for(int i = 0;i < 100;i++)
    {
        bb.writeShort(i);
    }
    for(int i = 0;i < 100;i++)
    {
        bb.readShort(i16);
        assert(i16 == i);
    }
    for(int i = 50;i < 1100;i++)
    {
        bb.writeInt(i);
    }
    for(int i = 50;i < 1100;i++)
    {
        bb.readInt(i32);
        //printf("i32 = %d and i = %d\n",i32,i);
        assert(i32 == i);
    }
    
    return 0;
}

