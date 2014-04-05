#ifndef XBEE_FRAME_STRUCTS_H
#define XBEE_FRAME_STRUCTS_H

#include <stdint.h>
#include <string>
#include "xbee_api_frame_defines.h"

namespace XbeeAPI {

// Packets to write

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
 * @brief The ExAddressingCmd struct holds the variable fields of the Explicit Addressing Command Xbee frame.
 */
struct ExAddressingCmd
{
    uint8_t FrameID;                ///< @brief FrameID is the UART data frame for the host to correlate with a subsequent ACK
    uint64_t DestAddress;           ///< @brief DestAddress is the 64-bit address of destination
    uint8_t SourceEP;               ///< @brief SourceEP is the source endpoint
    uint8_t DestEP;                 ///< @brief DestEP is the destination endpoint
    uint16_t ClusterID;             ///< @brief ClusterID is the cluster ID
    uint16_t ProfileID;             ///< @brief ProfileID is the profile ID
    uint8_t BroadcastRadius;        ///< @brief BroadcastRadius is the max # of hops a broadcast packet can traverse.
                                    /// A value of 0 represents the network max hops value.
    uint8_t TxOptions;              ///< @brief TxOptions is the transmit options bitfield
    uint8_t* PayloadStart;          ///< @brief PayloadStart points to the start of the payload (preallocated by client)
    uint16_t PayloadLength;         ///< @brief PayloadLength is the length of the payload
};

/**
 * @brief The LinkTest struct holds the variables used to construct an ExAddressingCmd frame for link testing
 */
struct LinkTestRequest
{
    uint64_t TestNodeA_Addr;        ///< @brief TestNodeA_Addr is the address of the first link endpoint
    uint64_t TestNodeB_Addr;        ///< @brief TestNodeB_Addr is the address of the second link endpoint
    uint16_t TestPayloadSize;       ///< @brief TestPayloadSize is the size of the test payload used to test the link
    uint16_t NumTestIterations;     ///< @brief NumTestIterations is the number of times the test payload should be transmitted

    uint8_t TxOptions;              ///< @brief TxOptions is the transmit options bitfield
};


// Packets to parse

/**
 * @brief The TxStatus struct holds the fields returned by the Transmit Status Xbee frame.
 */
struct TxStatus
{
    static const uint8_t ActualFrameSize = TXS_MIN_TOTALSIZE;

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
};

/**
 * @brief The ExRxIndicator struct holds the fields parsed from the Explicit Rx Indicator Xbee frame.
 */
struct ExRxIndicator
{
    uint64_t SourceAddress;
    uint8_t SourceEP;
    uint8_t DestEP;
    uint16_t ClusterID;
    uint16_t ProfileID;
    uint8_t RxOptions;

    uint8_t* PayloadStart;
    uint16_t ExpctdPayloadLen;
    uint32_t ActualFrameSize;
};


} // XbeeAPI

#endif // XBEE_FRAME_STRUCTS_H
