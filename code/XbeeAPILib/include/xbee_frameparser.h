#ifndef XBEE_FRAMEPARSER_H
#define XBEE_FRAMEPARSER_H

#include "xbee_api_basetypes.h"
#include "xbee_frame_params.h"

namespace XbeeAPI {

namespace FrameParser {

Frame::Type ParseFrameType(char* FrameStart);

TxRequestParams ParseTxRequest(char* FrameStart);

} // FrameParser

} // XbeeAPI

#endif // XBEE_FRAMEPARSER_H
