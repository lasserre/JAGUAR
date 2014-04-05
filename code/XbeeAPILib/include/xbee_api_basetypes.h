#ifndef XBEE_API_H
#define XBEE_API_H

#ifndef NULL
#define NULL 0
#endif

#include <stdint.h>
#include "xbee_frame_enums.h"

/** @file */

/** @namespace XbeeAPI
 * The XbeeAPI namespace is the top-level namespace for the XbeeAPI library
 */
namespace XbeeAPI
{

/**
 * @brief CalcFrameSpecificLength
 * @param FrameType
 * @param PayloadLength
 * @return length if valid type, 0 if unrecognized type
 */
uint16_t CalcFrameSpecificLength(const XBFrame::Type& FrameType, const uint16_t& PayloadLength);

/**
 * @brief CalcChecksum calculates the value of the checksum.
 * @param SizeWOChecksum is the size of the array not including the checksum byte
 * @param Start is the start of the array
 * @return checksum value
 */
uint8_t CalcChecksum(const int& SizeWOChecksum, uint8_t* Start);

/**
 * @brief CalcFrameChecksum calculates the checksum for a frame
 * @param FrameSize is the final size of XBee frame
 * @param FrameStart points to the start of the XBee frame
 * @return checksum value
 */
uint8_t CalcFrameChecksum(const XBFrame::FrameByteArray& Frame);

/**
 * @brief WriteXbeeFrameHeader writes start byte, length, and type fields to the beginning of the frame
 * @param FrameStart points to the start of the XBee frame (Frame[0] will be 0x7E)
 * @param FrameLen is the available size of the XBee frame (size of the array we are writing to)
 * @param LengthField is the value of the 16 bit length field in the XBee frame
 * @param FrameType is the type of the XBee frame
 */
XBFrame::WriteResult WriteXbeeFrameHeader(const XBFrame::FrameByteArray &Frame, const uint16_t &LENGTH_field,
                                          const XBFrame::Type& FrameType);

void WriteXbeeDestAddress(uint8_t* DestAddressStart, const uint64_t &DestAddress);
bool WriteFrameTypeField(const XBFrame::Type& FrameType, uint8_t *FrameTypeByte);
void WriteReservedField(uint8_t* ReservedStartByte);
void WriteChecksum(XBFrame::FrameByteArray *Frame);
void Write_uint16_t(uint16_t Bytes, uint8_t* DestStart);
void Write_uint32_t(uint32_t Bytes, uint8_t* DestStart);
void Write_uint64_t(uint64_t Bytes, uint8_t* DestStart);

}   // XbeeAPI

#endif // XBEE_API_H

