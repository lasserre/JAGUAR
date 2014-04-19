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
        if (XBLength != 0)
            return XBLength + XB_BASEFRAME_SIZE;
        else
            return 0;       // Return 0 if XBLength is 0...we have no frame-specific data! (unrealistic scenario)
    }

    /**
     * @brief SetFrameSize sets the LENGTH field (XBLength) from the entire frame size
     * @param FrameSize is the size of the entire Xbee frame
     */
    void SetFrameSize(const uint32_t& FrameSize)
    {
        if (FrameSize >= (uint32_t)XB_BASEFRAME_SIZE)
            XBLength = (uint16_t) (FrameSize - XB_BASEFRAME_SIZE);
        else
            XBLength = 0;   // Make sure we don't set it to a huge # (since this is unsigned...)
    }
};

/**
 * @brief The Type enum lists each Xbee frame type
 */
enum Type
{
    UnknownFrameType,       ///< @brief UnknownFrameType is returned for types not (yet) supported, or invalid types
    TransmitRequest,        ///< @brief TransmitRequest represents the Transmit Request frame
    TransmitStatus,         ///< @brief TransmitStatus represents the Transmit Status frame
    ReceivePacket,          ///< @brief ReceivePacket represents the Receive Packet frame
    ExAddrCmd,              ///< @brief ExAddrCmd represents the Explicit Addressing Command frame
    ExRxIndicator           ///< @brief ExRxIndicator represents the Explicit Rx Indicator frame
};

/**
 ** @brief The WriteResult enum specifies the possible return values for each of the write functions
 **
 ** Write Result Categories
 ** --
 ** Each WriteResult is placed in a category, all of which are enumerated in the WriteResCategory type.
 ** The meaning of the categories is given below.
 **
 ** - NIB (Not in bounds) - One or more of the fields that needs to be written falls outside of the given length
 ** - Success - The frame was written successfully.
 **
 ** Naming conventions
 ** --
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
    WR_NIB_CATEGORY_BEGIN,      ///< Marks beginning of NIB result category (do not return this as a result!)

    // Not in bounds results
    WR_XBHeader_NIB,               ///< Xbee-generic header is not in bounds
    WR_FRAMETYPE_NIB,              ///< FRAMETYPE byte is not in bounds
    //----------------------

    WR_SUCCESS_CATEGORY_BEGIN,  ///< Marks beginning of SUCCESS result category (do not return this as a result!)

    // Success results
    WR_WriteSuccess                ///< Write successful
};

enum WriteResCategory
{
    WRC_NIB,
    WRC_Success
};

/**
 ** @brief The ParseResult enum specifies the possible return values for each of the parse functions
 **
 ** Parse Result Categories
 ** --
 ** Each ParseResult is placed in a category, all of which are enumerated in the ParseResCategory type.
 ** The meaning of the categories is given below.
 **
 ** - Error - There is no way to continue parsing. Either clear the buffer, or try to find a valid start byte
 ** - NIB (Not in bounds) - One or more of the fields has not been received yet (or at least was not in the buffer)
 ** - Success - The frame was parsed successfully. However, there may be extra data in the buffer (the
 ** beginning of the next frame)
 **
 ** Naming conventions
 ** --
 ** The convention for the ParseResult enum names is as follows:
 **
 ** - PR_ResultDescription --- Generic result description
 ** - PR_BYTENAME_[result] --- Specific [result] for a given BYTENAME
 **
 ** with [result] for a given BYTENAME being:
 **
 ** - NIB --- Byte is not in bounds (e.g. does not fall within the bounds set by FrameLength)
 ** - INV --- Byte has invalid value
 **
 ** @note the PR_ prefix is used to distinguish possible naming clashes with the WriteResult enum type
 ** which is in the same namespace (similarly prefixed with WR_)
 **
 ** @note enum values with names of format PR_X_CATEGORY_BEGIN are not valid return results. These are simply
 ** used to allow us to classify result values into categories
 */
enum ParseResult
{
    PR_ERROR_CATEGORY_BEGIN,    ///< Marks beginning of ERROR result category (do not return this as a result!)

    // Error results
    PR_START_INV,                  ///< START byte is invalid
    PR_FRAMETYPE_INV,              ///< FRAMETYPE byte value is invalid (unrecognized)
    //----------------

    PR_NIB_CATEGORY_BEGIN,      ///< Marks beginning of NIB result category (do not return this as a result)

    // Not in bounds results
    PR_MinLenNIB,                  ///< The min # of bytes for the given frametype is not in bounds
    PR_LENGTH_NIB,                 ///< LENGTH byte is not in bounds
    PR_ADDRESS_NIB,                ///< ADDRESS bytes are not in bounds
    PR_FRAMETYPE_NIB,              ///< FRAMETYPE byte is not in bounds
    PR_RCVOPTS_NIB,                ///< RCVOPTS byte is not in bounds (Receive Packet frame)
    PR_PAYLOAD_NIB,                ///< PAYLOAD bytes are not in bounds
    PR_SRCENDPT_NIB,               ///< SRCENDPT byte is not in bounds
    PR_DSTENDPT_NIB,               ///< DSTENDPT byte is not in bounds
    PR_CLUSTERID_NIB,              ///< CLUSTERID byte is not in bounds
    PR_PROFILEID_NIB,              ///< PROFILEID byte is not in bounds
    PR_CHECKSUM_NIB,               ///< CHECKSUM byte is not in bounds
    //----------------

    PR_SUCCESS_CATEGORY_BEGIN,  ///< Marks beginning of SUCCESS result category (do not return this as a result!)

    // Success results
    PR_ParseSuccess,               ///< Parse successful
    PR_XtraBytes,                  ///< The size of the frame passed in to be parsed was larger than the frame actually parsed
    //----------------

    PR_LINK_TEST_CATEGORY_BEGIN,

    // Link Test results

    PR_LT_Success,
    PR_LT_XtraBytes,
    PR_LT_PLDLenDNM,                ///< Payload length does not match LTR_PAYLOAD_SIZE
    PR_LT_Fail
};

/**
 ** @brief The ParseResCategory enum specifies the possible categories of ParseResults to make processing ParseResults easier
 ** on client code.
 **
 ** @note The convention is to order the PRC_ values in alphabetical order, to make category checking
 ** straightforward
 **
 */
enum ParseResCategory
{
    PRC_Error,
    PRC_NIB,
    PRC_Success,
    PRC_LinkTestResult,
    PRC_UnknownCategory
};

}   // Frame

}   // XbeeAPI

#endif // XBEE_FRAME_ENUMS_H
