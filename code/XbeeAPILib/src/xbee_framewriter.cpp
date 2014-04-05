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

XBFrame::WriteResCategory GetWriteResultCategory(const XBFrame::WriteResult &Result)
{
    // Check the categories in order, since first match will return the category

    if (Result < XBFrame::WR_SUCCESS_CATEGORY_BEGIN)
        return XBFrame::WRC_NIB;        // No known category before this!
    else
        return XBFrame::WRC_Success;
}

uint16_t CalcEntireFrameLength(const XBFrame::Type& Type, const uint16_t& PayloadLength)
{
    return CalcFrameSpecificLength(Type, PayloadLength) + XB_BASEFRAME_SIZE;
}

void WriteTxRequestFrame(const TxRequest &Options, XBFrame::FrameByteArray& Frame)
{
    uint16_t frameSpecLen = CalcFrameSpecificLength(XBFrame::TransmitRequest, Options.PayloadLength);

    WriteXbeeFrameHeader(Frame, frameSpecLen, XBFrame::TransmitRequest);

    Frame.FrameStart[4] = 0x01;   // Frame ID

    WriteXbeeDestAddress(&Frame.FrameStart[5], Options.DestAddress);
    WriteReservedField(&Frame.FrameStart[13]);  // Reserved field

    Frame.FrameStart[15] = 0x00;  // Broadcast Radius (0 = max)
    Frame.FrameStart[16] = 0x00;  // Tx Options (bitfield)

    memcpy(&Frame.FrameStart[17], Options.PayloadStart, Options.PayloadLength);

    WriteChecksum(&Frame);

    return;
}

XBFrame::WriteResult WriteExAddressingCmdFrame(const ExAddressingCmd &Options, XBFrame::FrameByteArray &Frame,
                                               const bool& LinkTest /*= false*/)
{
    XBFrame::WriteResult result;

    uint16_t frameSpecLen = CalcFrameSpecificLength(XBFrame::ExAddrCmd, Options.PayloadLength);

    result = WriteXbeeFrameHeader(Frame, frameSpecLen, XBFrame::ExAddrCmd);

    if (GetWriteResultCategory(result) != XBFrame::WRC_Success)
        return result;

    Frame.FrameStart[EXA_FRAMEID_IDX] = Options.FrameID;

    WriteXbeeDestAddress(&Frame.FrameStart[EXA_DESTADDR_IDX], Options.DestAddress);
    WriteReservedField(&Frame.FrameStart[EXA_RESERVED_IDX]);    // Reserved

    Frame.FrameStart[EXA_SRCENDPT_IDX] = Options.SourceEP;
    Frame.FrameStart[EXA_DSTENDPT_IDX] = Options.DestEP;

    Write_uint16_t(Options.ClusterID, &Frame.FrameStart[EXA_CLUSTERID_IDX]);
    Write_uint16_t(Options.ProfileID, &Frame.FrameStart[EXA_PROFILEID_IDX]);

    Frame.FrameStart[EXA_BCSTRAD_IDX] = Options.BroadcastRadius;
    Frame.FrameStart[EXA_TXOPTIONS_IDX] = Options.TxOptions;

    // If LinkTest, the payload has already been written to!
    if (!LinkTest)
        memcpy(&Frame.FrameStart[EXA_PAYLOAD_IDX], Options.PayloadStart, Options.PayloadLength);

    WriteChecksum(&Frame);

    return XBFrame::WR_WriteSuccess;
}

XBFrame::WriteResult WriteTestLinkRequestFrame(const LinkTestRequest &Options, XBFrame::FrameByteArray &Frame)
{
    ExAddressingCmd DerivedOptions;
    DerivedOptions.DestAddress = Options.TestNodeA_Addr;
    DerivedOptions.BroadcastRadius = 0;
    DerivedOptions.ClusterID = 0x0014;
    DerivedOptions.ProfileID = 0xc105;
    DerivedOptions.SourceEP = LT_SRCENDPT_VAL;
    DerivedOptions.DestEP = LT_DSTENDPT_VAL;
    DerivedOptions.FrameID = 1;
    DerivedOptions.TxOptions = Options.TxOptions;
    DerivedOptions.PayloadLength = 12;

    // Point PayloadStart to beginning of payload in Frame
    DerivedOptions.PayloadStart = &Frame.FrameStart[EXA_PAYLOAD_IDX];

    // Go ahead and write payload (WriteExAddr...() call won't write payload 2x if we specify LinkTest = true)
    Write_uint64_t(Options.TestNodeB_Addr, &DerivedOptions.PayloadStart[LTQ_DESTADDR_PLDIDX]);
    Write_uint16_t(Options.TestPayloadSize, &DerivedOptions.PayloadStart[LTQ_PLDSIZE_PLDIDX]);
    Write_uint16_t(Options.NumTestIterations, &DerivedOptions.PayloadStart[LTQ_NUMITER_PLDIDX]);

    return WriteExAddressingCmdFrame(DerivedOptions, Frame, true);
}

} // FrameConstructor

} // XbeeAPI
