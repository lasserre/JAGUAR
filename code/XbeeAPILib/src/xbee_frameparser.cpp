#include "xbee_frameparser.h"

namespace XbeeAPI {

namespace FrameParser {

Frame::Type ParseFrameType(char *FrameStart)
{
    return Frame::TransmitRequest;
}

TxRequestParams ParseTxRequest(char *FrameStart)
{
    return TxRequestParams();
}

} // FrameParser

} // XbeeAPI
