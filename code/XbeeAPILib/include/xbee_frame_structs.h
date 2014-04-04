#ifndef XBEE_FRAME_STRUCTS_H
#define XBEE_FRAME_STRUCTS_H

#include <stdint.h>
#include <string>
#include "xbee_api_frame_defines.h"

namespace XbeeAPI {

/**
 * @brief The TxRequest struct holds the variable fields of the Transmit Request Xbee frame.
 */
struct TxRequest
{
    uint64_t DestAddress;
    uint8_t* PayloadStart;
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

    std::string GetDelivStatString() const;
    std::string GetDiscvStatString() const;
};

/**
 * @brief The RxPacket struct holds the fields parsed from the Receive Packet Xbee frame.
 */
struct RxPacket
{
    uint64_t SourceAddress;     ///< @brief SourceAddress is the 64-bit address of the sender
    uint8_t ReceiveOpts;        ///< @brief ReceiveOpts is an 8-bit enum value describing the mode of reception
    uint8_t* PayloadStart;      ///< @brief PayloadStart points to the start of the payload within the frame supplied
    uint16_t ExpctdPayloadLen;  ///< @brief ExpctdPayloadLen is the expected payload length based on the LENGTH field
    uint32_t ActualFrameSize;   ///< @brief ActualFrameSize is the actual frame size detected by parsing

    std::string GetRcvOptsString() const;
};



} // XbeeAPI

#endif // XBEE_FRAME_STRUCTS_H
