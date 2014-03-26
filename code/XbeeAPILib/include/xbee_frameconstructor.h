#ifndef XBEE_FRAMECONSTRUCTOR_H
#define XBEE_FRAMECONSTRUCTOR_H

#include "xbee_api_basetypes.h"

/** @file */

// XBee broadcast address
#define BROADCAST_ADDRESS 0x000000000000FFFF

namespace XbeeAPI {

struct TxRequest;

namespace FrameConstructor {

/**
 * @brief GetTxRequestFrameSize
 * @param Options
 * @return
 */
uint16_t GetTxRequestFrameSize(const TxRequest& Options);

/**
 * @brief WriteTxRequestFrame
 * @param Options
 * @param FrameStart
 */
void WriteTxRequestFrame(const TxRequest& Options, char* FrameStart);

} // FrameConstructor

} // XbeeAPI

#endif // XBEE_FRAMECONSTRUCTOR_H
