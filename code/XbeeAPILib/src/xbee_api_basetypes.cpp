#include "xbee_api_frame_defines.h"
#include "xbee_api_basetypes.h"

namespace XbeeAPI {

uint16_t CalcFrameSpecificLength(const Frame::Type &FrameType, const uint16_t &PayloadLength)
{
    switch (FrameType)
    {
    case Frame::TransmitRequest:
        return PayloadLength + FLO_TRANSMIT_REQUEST;
    default:
        return 0;
    }
}

uint8_t CalcChecksum(const int& Size, char *Start)
{
    uint8_t sum = 0;

    for (int i = 0; i < Size; i++)
        sum += Start[i];

    return (0xff - sum);
}

uint8_t CalcFrameChecksum(const int& FrameSize, char* FrameStart)
{
    return CalcChecksum(FrameSize - XB_FRAME_LENGTH, FrameStart + XB_HEADER_LENGTH);
}

void WriteXbeeFrameHeader(char *FrameStart, const uint16_t &LengthField, const Frame::Type& FrameType)
{
    FrameStart[0] = 0x7e;   // Start byte

    FrameStart[1] = ((LengthField >> 8) & 0x00ff);  // Length MSB
    FrameStart[2] = (LengthField & 0x00ff);         // Length LSB

    WriteFrameTypeField(FrameType, &FrameStart[3]); // Frame Type

    return;
}

void WriteXbeeDestAddress(char* DestAddressStart, uint64_t DestAddress)
{
    for (int j = 7; j >= 0; j--)
    {
        DestAddressStart[j] = (DestAddress & 0x00000000000000ff);   // Grab LSByte
        DestAddress >>= 8;   // Shift LSB out to the right
    }
}

void WriteFrameTypeField(const Frame::Type &FrameType, char* FrameTypeByte)
{
    switch (FrameType)
    {
    case Frame::TransmitRequest:
        *FrameTypeByte = FT_TRANSMIT_REQUEST;
        break;
    default:
        *FrameTypeByte = FT_DEFAULT_FRAMETYPE;
        break;
    }
}

}
