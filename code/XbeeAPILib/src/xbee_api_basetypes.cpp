#include "xbee_api_frame_defines.h"
#include "xbee_api_basetypes.h"

namespace XbeeAPI {

uint16_t CalcFrameSpecificLength(const XBFrame::Type &FrameType, const uint16_t &PayloadLength)
{
    switch (FrameType)
    {
    case XBFrame::TransmitRequest:
        return PayloadLength + TXR_BASEFRAME_SIZE;
    case XBFrame::ReceivePacket:
        return PayloadLength + RXP_BASEFRAME_SIZE;
    case XBFrame::ExAddrCmd:
        return PayloadLength + EXA_BASEFRAME_SIZE;
        break;
    default:
        return 0;
    }
}

uint8_t CalcChecksum(const int& SizeWOChecksum, uint8_t *Start)
{
    uint8_t sum = 0;

    for (int i = 0; i < SizeWOChecksum; i++)
        sum += Start[i];

    return (0xff - sum);
}

uint8_t CalcFrameChecksum(const XBFrame::FrameByteArray &Frame)
{
    return CalcChecksum(Frame.FrameSize() - XB_BASEFRAME_SIZE, Frame.FrameStart + XB_HEADER_SIZE);
}

XBFrame::WriteResult WriteXbeeFrameHeader(const XBFrame::FrameByteArray& Frame, const uint16_t& LENGTH_field,
                                          const XBFrame::Type& FrameType)
{
    if (Frame.FrameSize() < XB_HEADER_SIZE)     // Not even enough space to write the header!
        return XBFrame::WR_XBHeader_NIB;

    Frame.FrameStart[XB_START_IDX] = XB_START_VAL;   // Start byte

    Write_uint16_t(LENGTH_field, &Frame.FrameStart[XB_LENMSB_IDX]);

    if (!WriteFrameTypeField(FrameType, &Frame.FrameStart[XB_FRAMETYPE_IDX])) // Frame Type
        return XBFrame::WR_FRAMETYPE_NIB;

    return XBFrame::WR_WriteSuccess;
}

void WriteXbeeDestAddress(uint8_t* DestAddressStart, const uint64_t& DestAddress)
{
    Write_uint64_t(DestAddress, DestAddressStart);
}

bool WriteFrameTypeField(const XBFrame::Type &FrameType, uint8_t* FrameTypeByte)
{
    switch (FrameType)
    {
    case XBFrame::TransmitRequest:
        *FrameTypeByte = TXR_FRAMETYPE_ENUM;
        break;
    case XBFrame::ReceivePacket:
        *FrameTypeByte = RXP_FRAMETYPE_ENUM;
        break;
    case XBFrame::ExAddrCmd:
        *FrameTypeByte = EXA_FRAMETYPE_ENUM;
        break;
    default:
        return false;   // Unable to write FRAMETYPE field
    }

    return true;
}

void WriteReservedField(uint8_t *ReservedStartByte)
{
    static const uint16_t ReservedByte = 0xfffe;
    Write_uint16_t(ReservedByte, ReservedStartByte);
}

void WriteChecksum(XBFrame::FrameByteArray* Frame)
{
    Frame->FrameStart[Frame->FrameSize() - 1] = CalcFrameChecksum(*Frame);
}

/**
 * @brief Write_uint16_t
 * @param Bytes
 * @param DestStart
 * @note All of the Write_uintX_t() functions write the multi-byte number in network byte order
 */
void Write_uint16_t(uint16_t Bytes, uint8_t *DestStart)
{
    // Write in reverse order:
    // ---------------------
    // 1. LSB -> last index,
    // 2. ... ,
    // 3. MSB -> first index
    // ---------------------

    for (int j = (sizeof(Bytes) - 1); j >= 0; j--)
    {
        DestStart[j] = (Bytes & 0x00000000000000ff);   // Grab LSByte
        Bytes >>= 8;   // Shift LSB out to the right
    }
}

void Write_uint32_t(uint32_t Bytes, uint8_t *DestStart)
{
    for (int j = (sizeof(Bytes) - 1); j >= 0; j--)
    {
        DestStart[j] = (Bytes & 0x00000000000000ff);   // Grab LSByte
        Bytes >>= 8;   // Shift LSB out to the right
    }
}

void Write_uint64_t(uint64_t Bytes, uint8_t *DestStart)
{
    for (int j = (sizeof(Bytes) - 1); j >= 0; j--)
    {
        DestStart[j] = (Bytes & 0x00000000000000ff);   // Grab LSByte
        Bytes >>= 8;   // Shift LSB out to the right
    }
}

}   // XbeeAPI
