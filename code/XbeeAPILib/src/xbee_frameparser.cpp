#include "xbee_frame_structs.h"
#include "xbee_frameparser.h"

namespace XbeeAPI {

namespace FrameParser {

// ************************************************************************
// ************************************************************************
/// @namespace Utils provides utility functions for the library itself.
///
/// The function prototypes are not included in the header file, and are not meant for client code
///
namespace Utils {

/**
 * @brief SafeGetByte provides a safe method of access attempts into payload arrays. This method intends
 * to prevent out-of-bounds accesses, as well as a consistent, reusable interface for safely reading
 * frame fields.
 * @note This is not intended for client code use (hence, not in .h file)
 * @param FrameStart points to the start of the frame
 * @param ByteIndex is the index into the frame of the byte being accessed
 * @param FrameLen is the length of bytes received so far (not value of the LENGTH field within the frame)
 * @param Byte is set to the value of the byte being accessed
 * @return True if access is successful, false otherwise
 */
inline bool SafeReadByte(const XBFrame::FrameByteArray& Frame, const uint32_t &ByteIndex, uint8_t &Byte)
{
    // Check for sufficient length
    if (Frame.FrameSize() <= ByteIndex)
        return false;

    // Access byte "safely" and return true
    Byte = Frame.FrameStart[ByteIndex];
    return true;
}

inline bool CanSafelyReadBytes(const XBFrame::FrameByteArray& Frame, const uint32_t ArrStartIdx, const uint32_t& ArrLen)
{
    uint64_t MinFrameSize = ArrStartIdx + ArrLen;   // Ensure frame is big enough to read entire array

    if (Frame.FrameSize() < MinFrameSize)
        return false;   // Frame not big enough
    else
        return true;    // Frame size is sufficient to read array
}

/**
 * @brief CheckStartByte verifies that the frame start byte is valid
 * @param FrameStart points to the beginning of the frame
 * @return True if start byte is valid, false otherwise
 */
inline bool CheckStartByte(uint8_t* FrameStart)
{
    if (FrameStart[XB_START_IDX] == XB_START_VAL)
        return true;
    else
        return false;
}

inline bool ReadLengthField(const XBFrame::FrameByteArray& Frame, uint16_t& LENGTH_field)
{
    uint8_t LEN_MSB;
    uint8_t LEN_LSB;

    if (!CanSafelyReadBytes(Frame, XB_LENMSB_IDX, 2))
        return false;
    else
    {
        LEN_MSB = Frame.FrameStart[XB_LENMSB_IDX];
        LEN_LSB = Frame.FrameStart[XB_LENLSB_IDX];

        LENGTH_field = ((((uint16_t)LEN_MSB) << 8) & 0xff00) | ((uint16_t)LEN_LSB);

        return true;
    }
}

inline bool Read64BitAddrField(const XBFrame::FrameByteArray& Frame, const uint32_t& AddressIdx, uint64_t& AddressField)
{
    if (!CanSafelyReadBytes(Frame, AddressIdx, sizeof(AddressField)))
        return false;
    else
    {
        AddressField = 0;

        for (uint8_t i = 0; i < sizeof(AddressField); i++)
        {
            // Assign AddressField
            AddressField <<= 8;
            AddressField |= (0x00000000000000ff & Frame.FrameStart[AddressIdx + i]);
        }

        return true;
    }
}

/**
 * @brief CalcExpctdPayloadLength calculates the length of the payload we expect given the frame type and
 * value that was parsed from the LENGTH field of the Xbee frame.
 * @param FrameType is the type of frame
 * @param XBLength is the value of the LENGTH field parsed from the Xbee frame
 * @return Size of expected payload if given type expects a payload, false otherwise.
 */
inline uint16_t CalcExpctdPayloadSize(const XBFrame::Type& FrameType, const uint16_t& XBLength)
{
    switch (FrameType)
    {
    case XBFrame::ReceivePacket:
        return XBLength - RXP_BASEFRAME_SIZE;
    default:
        return -1;
    }
}

inline uint32_t CalcExpctdFrameSize(const uint16_t& XBLength)
{
    return XBLength + XB_BASEFRAME_SIZE;
}

}   // Utils
// ************************************************************************
// ************************************************************************

std::string GetParseResultString(const XBFrame::ParseResult &Result)
{
    switch (Result) {
    case XBFrame::PR_ParseSuccess:
        return "Success";
    case XBFrame::PR_MinLenNIB:
        return "Min # of bytes for FrameType not received";
    case XBFrame::PR_XtraBytes:
        return "Extra bytes passed in";
    case XBFrame::PR_START_INV:
        return "Start byte is invalid";
    case XBFrame::PR_LENGTH_NIB:
        return "Length field not in bounds";
    case XBFrame::PR_ADDRESS_NIB:
        return "Address field not in bounds";
    case XBFrame::PR_FRAMETYPE_INV:
        return "Invalid FRAMETYPE value";
    case XBFrame::PR_FRAMETYPE_NIB:
        return "FRAMETYPE byte not in bounds";
    case XBFrame::PR_RCVOPTS_NIB:
        return "Receive options field not in bounds";
    case XBFrame::PR_PAYLOAD_NIB:
        return "Payload not fully in bounds";
    case XBFrame::PR_CHECKSUM_NIB:
        return "Checksum not in bounds";
    default:
        return "Unrecognized ParseResult";
    }
}

XBFrame::ParseResCategory GetParseResultCategory(const XBFrame::ParseResult &Result)
{
    // Check the categories in order, since first match will return the category

    if (Result < XBFrame::PR_ERROR_CATEGORY_BEGIN)
        return XBFrame::PRC_UnknownCategory;        // No known category before this!

    if (Result < XBFrame::PR_NIB_CATEGORY_BEGIN)
        return XBFrame::PRC_Error;

    if (Result < XBFrame::PR_SUCCESS_CATEGORY_BEGIN)
        return XBFrame::PRC_NIB;

    else
        return XBFrame::PRC_Success;
}

XBFrame::ParseResult ParseFrameType(const XBFrame::FrameByteArray& Frame, XBFrame::Type& FrameType)
{
    FrameType = XBFrame::UnknownFrameType;    // Set FrameType to unknown in case we return an error before parsing type

    if (!Utils::CheckStartByte(Frame.FrameStart))
        return XBFrame::PR_START_INV;

    uint8_t typeByte;

    if (!Utils::SafeReadByte(Frame, XB_FRAMETYPE_IDX, typeByte))
        return XBFrame::PR_FRAMETYPE_NIB;

    switch (typeByte)
    {
    case TXR_FRAMETYPE_ENUM:
        FrameType = XBFrame::TransmitRequest;
        break;
    case TXS_FRAMETYPE_ENUM:
        FrameType = XBFrame::TransmitStatus;
        break;
    case RXP_FRAMETYPE_ENUM:
        FrameType = XBFrame::ReceivePacket;
        break;
    default:
        FrameType = XBFrame::UnknownFrameType;
        return XBFrame::PR_FRAMETYPE_INV;
    }

    return XBFrame::PR_ParseSuccess;
}

XBFrame::ParseResult ParseTxStatus(const XBFrame::FrameByteArray &Frame, TxStatus &Status)
{
    if (TXS_MIN_TOTALSIZE <= Frame.FrameSize())
    {
        // TxStatus frames are fixed size, so go ahead and read it!

        Status.TxRetryCount = Frame.FrameStart[TXS_RTRYCOUNT_IDX];
        Status.DelivStat = Frame.FrameStart[TXS_DELIVSTAT_IDX];
        Status.DiscvStat = Frame.FrameStart[TXS_DISCVSTAT_IDX];

        return XBFrame::PR_ParseSuccess;
    }
    else
        return XBFrame::PR_MinLenNIB;
}

XBFrame::ParseResult ParseRxPacket(const XBFrame::FrameByteArray& Frame, RxPacket &Packet)
{
    if (RXP_MIN_TOTALSIZE <= Frame.FrameSize())
    {
        // Minimum size of a RxPacket frame received!

        /* ---------- Read fields ---------- */

        // Read Length
        uint16_t XBLength;
        if (!Utils::ReadLengthField(Frame, XBLength))
            return XBFrame::PR_LENGTH_NIB;      // Length field not in bounds

        // Read 64-bit source address
        if (!Utils::Read64BitAddrField(Frame, RXP_SOURCEADDR_IDX, Packet.SourceAddress))
            return XBFrame::PR_ADDRESS_NIB;     // Address field not in bounds

        // Read Receive Options field
        if (!Utils::SafeReadByte(Frame, RXP_RCV_OPTS_IDX, Packet.ReceiveOpts))
            return XBFrame::PR_RCVOPTS_NIB;     // RxOptions field not in bounds

        // Calculate payload size
        Packet.ExpctdPayloadLen = Utils::CalcExpctdPayloadSize(XBFrame::ReceivePacket, XBLength);

        if (!Utils::CanSafelyReadBytes(Frame, RXP_PAYLOAD_IDX, Packet.ExpctdPayloadLen))
            return XBFrame::PR_PAYLOAD_NIB;     // Payload not fully in bounds
        else
            Packet.PayloadStart = &Frame.FrameStart[RXP_PAYLOAD_IDX];    // Set payload index

        /* ---------- End Read fields ---------- */

        // Check expected size vs. received size
        Packet.ActualFrameSize = Utils::CalcExpctdFrameSize(XBLength);

        if (Packet.ActualFrameSize < Frame.FrameSize())
            return XBFrame::PR_XtraBytes;
        else if (Packet.ActualFrameSize == Frame.FrameSize())
            return XBFrame::PR_ParseSuccess;
        else
            return XBFrame::PR_CHECKSUM_NIB;
    }
    else
        return XBFrame::PR_MinLenNIB;
}

} // FrameParser

} // XbeeAPI
