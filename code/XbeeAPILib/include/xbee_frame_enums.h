#ifndef XBEE_FRAME_ENUMS_H
#define XBEE_FRAME_ENUMS_H

#include <stdint.h>
#include "xbee_api_frame_defines.h"

namespace XbeeAPI {

namespace XBFrame
{

/**
 * @brief The FrameByteArray struct is used to group the frame pointer with the frame size, and
 * provides methods for converting from the entire frame size, to the value of the LENGTH
 * field in the Xbee frame.
 */
struct FrameByteArray
{
    uint8_t* FrameStart;    ///< Points to start of Xbee frame
    uint16_t XBLength;      ///< Value of LENGTH field in Xbee frame

    FrameByteArray(uint8_t* Start = NULL, const uint32_t& Size = 0) : FrameStart(Start)
      , XBLength(0)
    {
        SetFrameSize(Size);
    }

    /**
     * @brief FrameSize calculates the entire Xbee frame size from the LENGTH field (XBLength)
     * @return The size of the entire Xbee frame
     */
    uint32_t FrameSize() const
    {
        return XBLength + XB_BASEFRAME_SIZE;
    }

    /**
     * @brief SetFrameSize sets the LENGTH field (XBLength) from the entire frame size
     * @param FrameSize is the size of the entire Xbee frame
     */
    void SetFrameSize(const uint32_t& FrameSize)
    {
        XBLength = (uint16_t) (FrameSize - XB_BASEFRAME_SIZE);
    }
};

/**
 * @brief The Type enum lists each Xbee frame type
 */
enum Type
{
    UnknownFrameType,
    TransmitRequest,
    TransmitStatus,
    ReceivePacket
};

/**
 ** @brief The WriteResult enum specifies the possible return values for each of the write functions
 **
 ** The convention for the WriteResult enum names is as follows:
 **
 ** - ResultDescription --- Generic result description
 ** - BYTENAME_[result] --- Specific [result] for a given BYTENAME
 ** - SECTION_[result] --- Specific [result] for a given SECTION
 **
 ** with [result] being:
 **
 ** - NIB --- Byte is Not In Bounds (e.g. does not fall within the bounds set by FrameLength)
 **
 ** @note the WR_ prefix is used to distinguish possible naming clashes with the ParseResult enum type
 ** which is in the same namespace (similarly prefixed with PR_)
 */
enum WriteResult
{
    // Generic
    WR_WriteSuccess,               ///< Write successful

    // Section/Byte-specific
    WR_XBHeader_NIB,               ///< Xbee-generic header is not in bounds
    WR_FRAMETYPE_NIB               ///< FRAMETYPE byte is not in bounds
};

/**
 ** @brief The ParseResult enum specifies the possible return values for each of the parse functions
 **
 ** The convention for the ParseResult enum names is as follows:
 **
 ** - PR_ResultDescription --- Generic result description
 ** - PR_BYTENAME_[result] --- Specific [result] for a given BYTENAME
 **
 ** with [result] for a given BYTENAME being:
 **
 ** - NIB --- Byte is not in bounds (e.g. does not fall within the bounds set by FrameLength)
 ** - INV --- Byte has invalid value
 ** - UNR --- Unrecognized
 **
 ** @note the PR_ prefix is used to distinguish possible naming clashes with the WriteResult enum type
 ** which is in the same namespace (similarly prefixed with WR_)
 */
enum ParseResult
{
    // Generic
    PR_ParseSuccess,               ///< Parse successful
    PR_MinLenNIB,                  ///< The min # of bytes for the given frametype is not in bounds
    PR_XtraBytes,                  ///< The size of the frame passed in to be parsed was larger than the frame actually parsed

    // Byte-specific
    PR_START_INV,                  ///< START byte is invalid
    PR_LENGTH_NIB,                 ///< LENGTH byte is not in bounds
    PR_ADDRESS_NIB,                ///< ADDRESS bytes are not in bounds
    PR_FRAMETYPE_UNR,              ///< FRAMETYPE byte value is unrecognized
    PR_FRAMETYPE_NIB,              ///< FRAMETYPE byte is not in bounds
    PR_RCVOPTS_NIB,                ///< RCVOPTS byte is not in bounds (Receive Packet frame)
    PR_PAYLOAD_NIB,                ///< PAYLOAD bytes are not in bounds
    PR_CHECKSUM_NIB                ///< CHECKSUM byte is not in bounds
};

}   // Frame

}   // XbeeAPI

#endif // XBEE_FRAME_ENUMS_H
