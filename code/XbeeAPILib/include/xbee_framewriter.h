#ifndef XBEE_FRAMECONSTRUCTOR_H
#define XBEE_FRAMECONSTRUCTOR_H

#include "xbee_api_basetypes.h"
#include <string>

/** @file */

// XBee broadcast address
#define BROADCAST_ADDRESS 0x000000000000FFFF

namespace XbeeAPI {

struct TxRequest;

namespace FrameWriter {

/**
 * @brief GetWriteResultString translates a WriteResult into a readable string
 * @param Result is the WriteResult to translate
 * @return std::string corresponding to Result
 */
std::string GetWriteResultString(const XBFrame::WriteResult& Result);

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
 * @param FrameLen
 */
void WriteTxRequestFrame(const TxRequest& Options, const XBFrame::FrameByteArray &Frame);

} // FrameConstructor

} // XbeeAPI

#endif // XBEE_FRAMECONSTRUCTOR_H
