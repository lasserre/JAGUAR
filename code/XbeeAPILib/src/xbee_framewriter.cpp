#include "xbee_frame_structs.h"
#include "xbee_framewriter.h"
#include <cstring>

namespace XbeeAPI {

namespace FrameWriter {

std::string GetWriteResultString(const XBFrame::WriteResult &Result)
{
    switch (Result)
    {
    case XBFrame::WR_WriteSuccess:
        return "Write successful";
    case XBFrame::WR_XBHeader_NIB:
        return "Xbee-generic header not in bounds";
    case XBFrame::WR_FRAMETYPE_NIB:
        return "FRAMETYPE byte not in bounds";
    default:
        return "Unrecognized WriteResult";
    }
}

uint16_t GetTxRequestFrameSize(const TxRequest& Options)
{
    return CalcFrameSpecificLength(XBFrame::TransmitRequest, Options.PayloadLength) + XB_BASEFRAME_SIZE;
}

void WriteTxRequestFrame(const TxRequest &Options, const XBFrame::FrameByteArray& Frame)
{
    uint16_t frameSpecLen = CalcFrameSpecificLength(XBFrame::TransmitRequest, Options.PayloadLength);

    WriteXbeeFrameHeader(Frame, frameSpecLen, XBFrame::TransmitRequest);

    Frame.FrameStart[4] = 0x01;   // Frame ID

    WriteXbeeDestAddress(&Frame.FrameStart[5], Options.DestAddress);

    Frame.FrameStart[13] = 0xff;  // Reserved
    Frame.FrameStart[14] = 0xfe;
    Frame.FrameStart[15] = 0x00;  // Broadcast Radius (0 = max)
    Frame.FrameStart[16] = 0x00;  // Tx Options (bitfield)

    memcpy(&Frame.FrameStart[17], Options.PayloadStart, Options.PayloadLength);

    uint16_t frameLength = GetTxRequestFrameSize(Options);
    Frame.FrameStart[frameLength - 1] = CalcFrameChecksum(Frame);

    return;
}

} // FrameConstructor

} // XbeeAPI
