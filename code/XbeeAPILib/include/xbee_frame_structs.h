#ifndef XBEE_FRAME_STRUCTS_H
#define XBEE_FRAME_STRUCTS_H

#include <stdint.h>
#include "xbee_api_frame_defines.h"

namespace XbeeAPI {

/**
 * @brief The TxRequest struct holds the variable fields of the Transmit Request Xbee frame.
 */
struct TxRequest
{
    uint64_t DestAddress;
    char* PayloadStart;
    uint16_t PayloadLength;
};

/**
 * @brief The TxStatus struct holds the fields returned by the Transmit Status Xbee frame.
 */
struct TxStatus
{
    uint8_t TxRetryCount;
    uint8_t DelivStat;
    uint8_t DiscvStat;
};

} // XbeeAPI

#endif // XBEE_FRAME_STRUCTS_H
