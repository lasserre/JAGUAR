#ifndef XBEE_FRAMECONSTRUCTOR_H
#define XBEE_FRAMECONSTRUCTOR_H

#include "xbee_api_basetypes.h"
#include <string>

/** @file */

// XBee broadcast address
#define BROADCAST_ADDRESS 0x000000000000FFFF

namespace XbeeAPI {

struct TxRequest;
struct ExAddressingCmd;
struct LinkTestRequest;

namespace FrameWriter {

/**
 * @brief GetWriteResultString translates a WriteResult into a readable string
 * @param Result is the WriteResult to translate
 * @return std::string corresponding to Result
 */
std::string GetWriteResultString(const XBFrame::WriteResult& Result);

/**
 * @brief GetParseResultCategory
 * @param Result
 * @return
 */
XBFrame::WriteResCategory GetWriteResultCategory(const XBFrame::WriteResult &Result);

/**
 * @brief CalcEntireFrameLength
 * @param Type
 * @param PayloadLength
 * @return
 */
uint16_t CalcEntireFrameLength(const XBFrame::Type& Type, const uint16_t& PayloadLength);

/**
 * @brief CalcLinkTestFrameLength
 * @return
 */
inline uint16_t CalcLinkTestFrameLength() { return EXA_MIN_TOTALSIZE + LTQ_PAYLOAD_SIZE; }

/**
 * @brief WriteTxRequestFrame
 * @param Options
 * @param Frame is the fully allocated frame object that will be written to
 */
void WriteTxRequestFrame(const TxRequest& Options, XBFrame::FrameByteArray &Frame);

/**
 * @brief WriteExAddressingCmdFrame
 * @param Options
 * @param Frame
 */
XBFrame::WriteResult WriteExAddressingCmdFrame(const ExAddressingCmd& Options, XBFrame::FrameByteArray &Frame,
                                               const bool &LinkTest = false);

/**
 * @brief WriteTestLinkRequestFrame
 * @param Options
 * @param Frame
 * @return
 */
XBFrame::WriteResult WriteTestLinkRequestFrame(const LinkTestRequest& Options, XBFrame::FrameByteArray &Frame);

} // FrameConstructor

} // XbeeAPI

#endif // XBEE_FRAMECONSTRUCTOR_H
