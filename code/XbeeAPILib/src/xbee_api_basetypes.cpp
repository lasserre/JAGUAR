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

    Frame.FrameStart[XB_LENMSB_IDX] = ((LENGTH_field >> 8) & 0x00ff);  // Length MSB
    Frame.FrameStart[XB_LENLSB_IDX] = (LENGTH_field & 0x00ff);         // Length LSB

    if (!WriteFrameTypeField(FrameType, &Frame.FrameStart[XB_FRAMETYPE_IDX])) // Frame Type
        return XBFrame::WR_FRAMETYPE_NIB;

    return XBFrame::WR_WriteSuccess;
}

void WriteXbeeDestAddress(uint8_t* DestAddressStart, uint64_t DestAddress)
{
    for (int j = 7; j >= 0; j--)
    {
        DestAddressStart[j] = (DestAddress & 0x00000000000000ff);   // Grab LSByte
        DestAddress >>= 8;   // Shift LSB out to the right
    }
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
    default:
        return false;   // Unable to write FRAMETYPE field
    }

    return true;
}

}   // XbeeAPI
