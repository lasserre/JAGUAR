#ifndef XBEE_FRAMEPARSER_H
#define XBEE_FRAMEPARSER_H

#include <string>
#include "xbee_api_basetypes.h"

/** @file */

namespace XbeeAPI {

struct TxStatus;
struct RxPacket;
struct ExRxIndicator;

namespace FrameParser {

/**
 * @brief GetParseResultString translates a ParseResult code into a readable string
 * @param Result is the ParseResult to translate
 * @return std::string corresponding to Result
 */
std::string GetParseResultString(const XBFrame::ParseResult& Result);

/**
 * @brief GetParseResultCategory allows client code to look up the result category for the given result,
 * to make further processing more straightforward. (e.g. you don't have to handle EVERY result type if
 * you don't need to!)
 * @param Result is the ParseResult to look up
 * @return The ParseResCategory for Result
 */
XBFrame::ParseResCategory GetParseResultCategory(const XBFrame::ParseResult& Result);

/**
 * @brief GetRcvOptsString
 * @param ReceiveOpts
 * @return
 */
std::string GetRcvOptsString(const uint8_t& ReceiveOpts);

/**
 * @brief ParseFrameType parses the type field of an Xbee frame
 * @param FrameStart points to the start of the Xbee frame
 * @param FrameLen is the length of the received frame
 * @param FrameType is set to the type of the frame that is parsed
 * @return ParseResult result of parse attempt
 */
XBFrame::ParseResult ParseFrameType(const XBFrame::FrameByteArray& Frame, XBFrame::Type &FrameType);

/**
 * @brief ParseTxRequest parses an Xbee frame known to be of type Transmit Request.
 * @param FrameStart points to the start byte of the Xbee Transmit Request frame to be parsed
 * @param FrameLen is the length of the received frame
 * @param Status is set to the field values parsed from the frame
 * @return ParseResult result of parse attempt
 */
XBFrame::ParseResult ParseTxStatus(const XBFrame::FrameByteArray& Frame, TxStatus& Status);

/**
 * @brief ParseRxPacket parses an Xbee frame known to be of type Receive Packet
 * @param FrameStart points to the start byte of the Xbee Receive Packet frame to be parsed
 * @param FrameLen is the length of the received frame
 * @param Packet is set to the field values parsed from the frame
 * @return ParseResult result of parse attempt
 */
XBFrame::ParseResult ParseRxPacket(const XBFrame::FrameByteArray &Frame, RxPacket& Packet);

/**
 * @brief ParseExRxIndicator
 * @param Frame
 * @param Indicator
 * @return
 */
XBFrame::ParseResult ParseExRxIndicator(const XBFrame::FrameByteArray &Frame, ExRxIndicator& Indicator);

} // FrameParser

} // XbeeAPI

#endif // XBEE_FRAMEPARSER_H
