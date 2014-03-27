#ifndef XBEE_FRAMEPARSER_H
#define XBEE_FRAMEPARSER_H

#include "xbee_api_basetypes.h"

/** @file */

namespace XbeeAPI {

struct TxStatus;
struct RxPacket;

namespace FrameParser {

/**
 * @brief The ParseResult enum specifies the possible return values for each of the parse functions
 */
enum ParseResult
{
    Success     ///< Parse successful
};

/**
 * @brief ParseFrameType parses the type field of an Xbee frame
 * @param FrameStart points to the start of the Xbee frame
 * @param FrameLen is the length of the received frame
 * @param FrameType is set to the type of the frame that is parsed
 * @return ParseResult result
 */
ParseResult ParseFrameType(char* FrameStart, const int &FrameLen, Frame::Type &FrameType);

/**
 * @brief ParseTxRequest parses an Xbee frame known to be of type Transmit Request.
 * @param FrameStart points to the start byte of the Xbee Transmit Request frame to be parsed
 * @param FrameLen is the length of the received frame
 * @param Status is set to the field values parsed from the frame
 * @return True if frame parsing was successful, false otherwise
 */
ParseResult ParseTxStatus(char* FrameStart, const int &FrameLen, TxStatus& Status);

/**
 * @brief ParseRxPacket parses an Xbee frame known to be of type Receive Packet
 * @param FrameStart points to the start byte of the Xbee Receive Packet frame to be parsed
 * @param FrameLen is the length of the received frame
 * @param Packet is set to the field values parsed from the frame
 * @return True if frame parsing was successful, false otherwise
 */
ParseResult ParseRxPacket(char* FrameStart, const int& FrameLen, RxPacket& Packet);

} // FrameParser

} // XbeeAPI

#endif // XBEE_FRAMEPARSER_H
