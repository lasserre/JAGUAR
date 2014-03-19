#ifndef XBEEPACKETPARSER_H
#define XBEEPACKETPARSER_H

#include "xbee_api.h"

using namespace XbeeAPI;

namespace XbeePacketParser
{
    XbeePacket Parse(const int& PacketLength, char* PacketStart);
}

#endif // XBEEPACKETPARSER_H
