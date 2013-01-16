
#include "packet.h"

namespace snet
{
    ///////////////////////////////////////////
    //ResponsePacket
    ///////////////////////////////////////////
    bool ResponsePacket::write(DataOutputStream *os)
    {
        return os->writeShort(m_result) == 2;
    }
    bool ResponsePacket::read(DataInputStream *is)
    {
        return is->readShort(m_result) == 2;
    }
    
    ///////////////////////////////////////////
    //PingPacket
    ///////////////////////////////////////////
    bool PingPacket::write(DataOutputStream *os)
    {
        return os->writeLong(m_time) == 8;
    }
    bool PingPacket::read(DataInputStream *is)
    {
        return is->readLong(m_time) == 8;
    }
    
    //////////////////////////////////////////
    //DefaultPacketFactory
    //////////////////////////////////////////
    BasePacket* DefaultPacketFactory::createPacket(sint32 opCode)
    {
        switch(opCode)
        {
        case PING_CODE:
        {
            return new PingPacket;
        }
        default :
            return NULL;
        }
        return NULL;
    }
}
