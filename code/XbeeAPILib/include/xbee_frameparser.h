#ifndef XBEE_FRAMEPARSER_H
#define XBEE_FRAMEPARSER_H

#include "xbee_api_basetypes.h"

/** @file */

namespace XbeeAPI {

struct TxStatus;

namespace FrameParser {

/**
 * @brief ParseFrameType parses the type field of an Xbee frame
 * @param FrameStart points to the start of the Xbee frame
 * @return The frame type
 */
Frame::Type ParseFrameType(char* FrameStart);

/**
 * @brief ParseTxRequest parses an Xbee frame known to be of type Transmit Request.
 * @param FrameStart points to the start byte of the Xbee Transmit Request frame to be parsed
 * @return The TxStatusParams struct holding the fields parsed from *FrameStart
 */
TxStatus ParseTxStatus(char* FrameStart);

} // FrameParser

} // XbeeAPI

#endif // XBEE_FRAMEPARSER_H
