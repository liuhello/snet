
#include "socket/stream.h"
#include "config.h"
#include <cstdlib>
#include <cstdio>
#include <assert.h>

int main(int argc,char** argv)
{
    snet::ByteArrayOutputStream os;
    os.writeByte(1);
    os.writeShort(2);
    os.writeInt(3);
    os.writeLong(4);
    os.writeLong(1111111114);
    sint8 i8;
    sint16 i16;
    sint32 i32;
    sint64 i64;
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
    assert(i64 == 1111111114);
    return 0;
}

