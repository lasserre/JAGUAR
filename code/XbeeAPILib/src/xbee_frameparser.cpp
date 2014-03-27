#include "xbee_frame_structs.h"
#include "xbee_frameparser.h"

namespace XbeeAPI {

namespace FrameParser {

ParseResult ParseFrameType(char *FrameStart, const int& FrameLen, Frame::Type& FrameType)
{
    if (FrameLen <= XB_FRAMETYPE_IDX)
    {
        // FrameType byte not received yet!
        FrameType = Frame::UnknownFrameType;
        return Success;    /// @todo RETURN ERROR
    }

    uint8_t typeByte = FrameStart[XB_FRAMETYPE_IDX];

    switch (typeByte)
    {
    case TXR_FRAMETYPE_ENUM:
        FrameType = Frame::TransmitRequest;
        break;
    case TXS_FRAMETYPE_ENUM:
        FrameType = Frame::TransmitStatus;
        break;
    default:
        FrameType = Frame::UnknownFrameType;
        break;
    }

    return Success;
}

ParseResult ParseTxStatus(char *FrameStart, const int& FrameLen, TxStatus &Status)
{
    if (TXS_MIN_TOTALSIZE <= FrameLen)
    {
        // TxStatus frames are fixed size, so go ahead and read it!

        Status.TxRetryCount = FrameStart[TXS_RTRYCOUNT_IDX];
        Status.DelivStat = FrameStart[TXS_DELIVSTAT_IDX];
        Status.DiscvStat = FrameStart[TXS_DISCVSTAT_IDX];

        return Success;
    }

    return Success;     /// @todo RETURN ERROR HERE
}

ParseResult ParseRxPacket(char *FrameStart, const int &FrameLen, RxPacket &Packet)
{
    if (RXP_MIN_TOTALSIZE <= FrameLen)
    {
        // Minimum size of a RxPacket frame received!
    }
}

} // FrameParser

} // XbeeAPI
