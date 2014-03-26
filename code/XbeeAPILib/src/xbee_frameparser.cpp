#include "xbee_frame_structs.h"
#include "xbee_frameparser.h"

namespace XbeeAPI {

namespace FrameParser {

Frame::Type ParseFrameType(char *FrameStart)
{
    uint8_t typeByte = FrameStart[XB_FRAMETYPE_IDX];

    switch (typeByte)
    {
    case FT_TRANSMIT_REQUEST:
        return Frame::TransmitRequest;
    case FT_TRANSMIT_STATUS:
        return Frame::TransmitStatus;
    default:
        return Frame::UnknownFrameType;
    }
}

TxStatus ParseTxRequest(char *FrameStart)
{
    TxStatus txStatus;

    txStatus.TxRetryCount = FrameStart[7];
    //txStatus.DelivStat = FrameStart[];
    //txStatus.DiscvStat = FrameStart[];

    return txStatus;
}

} // FrameParser

} // XbeeAPI
