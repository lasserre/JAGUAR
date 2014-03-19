#ifndef XBEE_PACKETCREATOR_H
#define XBEE_PACKETCREATOR_H

#include "xbee_api.h"

namespace XbeePacketCreator
{
    XbeeAPI::XbeeFrame CreateTransmitRequest(const int& PayloadLength, char* Payload, const int& DestAddress);
}

#endif // XBEE_PACKETCREATOR_H

//class XBTxRequest : public XbeeFrame
//{
//    XBTxRequest(const int& PayloadLength, char* Payload, const int& DestAddress);
//};
