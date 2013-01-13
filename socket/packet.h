
#include "socket/stream.h"
#include "config.h"

#ifndef __PACKET_H__
#define __PACKET_H__

namespace snet
{    
    struct PacketHearder
    {
        sint32 m_opCode;
        suint8 m_flag;
        sint32 m_len;
    };
    
    class BasePacket
    {
    public:
        BasePacket(sint32 code):m_opCode(code){};
        virtual bool write(DataOutputStream* os) = 0;
        virtual bool read(DataInputStream* is) = 0;
        sint32 getOpCode(){return m_opCode;}
    protected:
        sint32 m_opCode;
    };
    
    class PacketFactory
    {
    public:
        virtual BasePacket* createPacket(sint32 opCode) = 0;
    };
    
    class PingPacket : public BasePacket
    {
    public:
        PingPacket(sint64 time=0):BasePacket(PING_CODE),m_time(time){};
        ~PingPacket(){}
        bool write(DataOutputStream* os);
        bool read(DataInputStream* is);
        sint64 getTime(){return m_time;}
        void setTime(sint64 time){m_time = time;}
    protected:
        sint64 m_time;
    };
    
    class DefaultPacketFactory : public PacketFactory
    {
    public:
        BasePacket* createPacket(sint32 opCode);
    };
}
#endif
