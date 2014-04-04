#include "xbee_frame_structs.h"
#include "xbee_api_frame_defines.h"

namespace XbeeAPI {

std::string TxStatus::GetDelivStatString() const
{
    switch (DelivStat)
    {
    case TXS_DELIVSTAT_SUCCESS:
        return "Success";
    case TXS_DELIVSTAT_INV_END:
        return "InvalidDestinationEndpoint";
    case TXS_DELIVSTAT_MACFAIL:
        return "MAC_ACK_Failure";
    case TXS_DELIVSTAT_NETFAIL:
        return "NetworkACKFailure";
    case TXS_DELIVSTAT_PLD2BIG:
        return "DataPayloadTooLarge";
    case TXS_DELIVSTAT_RT_NFND:
        return "RouteNotFound";
    default:
        return "UnrecognizedDeliveryStatus";
    }
}

std::string TxStatus::GetDiscvStatString() const
{
    switch (DiscvStat)
    {
    case TXS_DISCVSTAT_NDOVHD:
        return "NoDiscoveryOverhead";
    case TXS_DISCVSTAT_RTDSCV:
        return "RouteDiscovery";
    default:
        return "UnrecognizedDiscoveryStatus";
    }
}

std::string RxPacket::GetRcvOptsString() const
{
    switch (ReceiveOpts)
    {
    case RXP_RCV_OPTS_ACK:
        return "PacketAcknowledged";
    case RXP_RCV_OPTS_BCP:
        return "PacketWasBroadcast";
    default:
        return "UnrecognizedReceiveOption";
    }
}

}
