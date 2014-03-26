#ifndef XBEE_API_H
#define XBEE_API_H

#ifndef NULL
#define NULL 0
#endif

#include <stdint.h>

/** @file */

/** @namespace XbeeAPI
 * The XbeeAPI namespace is the top-level namespace for the XbeeAPI library
 */
namespace XbeeAPI
{
    namespace Frame
    {
        /**
         * @brief The Type enum lists each Xbee frame type
         */
        enum Type
        {
            UnknownFrameType,
            TransmitRequest,
            TransmitStatus
        };
    }

    /**
     * @brief CalcFrameSpecificLength
     * @param FrameType
     * @param PayloadLength
     * @return length if valid type, 0 if unrecognized type
     */
    uint16_t CalcFrameSpecificLength(const Frame::Type& FrameType, const uint16_t& PayloadLength);

    /**
     * @brief CalcChecksum calculates the value of the checksum.
     * @param Size is the size of the array
     * @param Start is the start of the array
     * @return checksum value
     */
    uint8_t CalcChecksum(const int& Size, char* Start);

    /**
     * @brief CalcFrameChecksum calculates the checksum for a frame
     * @param FrameSize is the final size of XBee frame
     * @param FrameStart points to the start of the XBee frame
     * @return checksum value
     */
    uint8_t CalcFrameChecksum(const int& FrameSize, char* FrameStart);

    /**
     * @brief WriteXbeeFrameHeader writes start byte, length, and type fields to the beginning of the frame
     * @param FrameStart points to the start of the XBee frame (Frame[0] will be 0x7E)
     * @param LengthField is the value of the 16 bit length field in the XBee frame
     * @param FrameType is the type of the XBee frame
     */
    void WriteXbeeFrameHeader(char* FrameStart, const uint16_t& LengthField, const Frame::Type& FrameType);

    void WriteXbeeDestAddress(char* DestAddressStart, uint64_t DestAddress);
    void WriteFrameTypeField(const Frame::Type& FrameType, char* FrameTypeByte);
}

#endif // XBEE_API_H

