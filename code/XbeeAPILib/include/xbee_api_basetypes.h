#ifndef XBEE_API_H
#define XBEE_API_H

#ifndef NULL
#define NULL 0
#endif

#include <stdint.h>

#define BASE_HEADER_LENGTH 3 // start byte and 2 length bytes
#define BASE_FRAME_LENGTH  4 // BASE_HEADER_LENGTH + checksum byte

/* ---------- FT - Frame Type Byte Definitions (hex) ---------- *
 * Frame Type is the hex value corresponding to the frame
 * type. This value is written to the Frame Type field in
 * the actual Xbee frame.
 * ------------------------------------------------------- */
#define FT_DEFAULT_FRAMETYPE 0x00
#define FT_TRANSMIT_REQUEST 0x10

/* ---------- PO - Payload Offsets (decimal) ---------- *
 * Payload Offset is the offset into the Xbee frame
 * that points to the start of the payload.
 * ----------------------------------------------- */
#define PO_TRANSMIT_REQUEST 17

/* ---------- FLO - Frame Length Offsets (decimal) ---------- *
 * Frame Length Offset is the fixed-length offset to add
 * to the size of the payload to get the value of
 * the Length field in the Xbee frame.
 *
 * FLO + PS = LEN
 *
 * where:
 *   FLO - Frame Length Offset
 *   PS - Payload size
 *   LEN - Length field in Xbee frame
 * ---------------------------------------------- */
#define FLO_TRANSMIT_REQUEST 14

namespace XbeeAPI
{
    namespace Frame
    {
        /**
         * @brief The Type enum lists each Xbee frame type
         */
        enum Type
        {
            TransmitRequest
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

