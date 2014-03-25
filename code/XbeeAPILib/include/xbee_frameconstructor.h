#ifndef XBEE_FRAMECONSTRUCTOR_H
#define XBEE_FRAMECONSTRUCTOR_H

#include "xbee_api_basetypes.h"
#include "xbee_frame_params.h"

// XBee broadcast address
#define BROADCAST_ADDRESS 0x000000000000FFFF

namespace XbeeAPI {

namespace FrameConstructor {

uint16_t GetTxRequestFrameSize(const TxRequestParams& Options);

void WriteTxRequestFrame(const TxRequestParams& Options, char* FrameStart);

} // FrameConstructor

} // XbeeAPI

#endif // XBEE_FRAMECONSTRUCTOR_H
