#include "xbee_frame_structs.h"
#include "xbee_frameconstructor.h"
#include <cstring>

namespace XbeeAPI {

namespace FrameConstructor {

uint16_t GetTxRequestFrameSize(const TxRequest& Options)
{
    return CalcFrameSpecificLength(Frame::TransmitRequest, Options.PayloadLength) + XB_FRAME_LENGTH;
}

void WriteTxRequestFrame(const TxRequest &Options, char *FrameStart)
{
    WriteXbeeFrameHeader(FrameStart, CalcFrameSpecificLength(Frame::TransmitRequest, Options.PayloadLength), Frame::TransmitRequest);

    FrameStart[4] = 0x01;   // Frame ID

    WriteXbeeDestAddress(&FrameStart[5], Options.DestAddress);

    FrameStart[13] = 0xff;  // Reserved
    FrameStart[14] = 0xfe;
    FrameStart[15] = 0x00;  // Broadcast Radius (0 = max)
    FrameStart[16] = 0x00;  // Tx Options (bitfield)

    memcpy(&FrameStart[17], Options.PayloadStart, Options.PayloadLength);

    uint16_t frameLength = GetTxRequestFrameSize(Options);
    FrameStart[frameLength - 1] = CalcFrameChecksum(frameLength, FrameStart);

    return;
}

} // FrameConstructor

} // XbeeAPI
