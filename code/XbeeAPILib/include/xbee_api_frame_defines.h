#ifndef XBEE_API_FRAME_DEFINES_H
#define XBEE_API_FRAME_DEFINES_H

/** @file */

/** **************************************************
 ** @defgroup XB_GENERIC_DEFS Xbee Frame Definitions
 **
 ** @brief Xbee-generic definitions
 **
 ** @{
 **/

#define XB_HEADER_LENGTH 3  /**< Start byte + 2 length bytes */
#define XB_FRAME_LENGTH  4  /**< XB_HEADER_LENGTH + checksum byte */

#define XB_START_IDX 0
#define XB_LEN_IDX 1
#define XB_FRAMETYPE_IDX 3

/**
 ** @}
 **/

/** **************************************************
 ** @defgroup FRAME_TYPE_FIELDS Frame Type Byte Fields
 **
 ** @brief Frame Type is the hex value corresponding to the frame type.
 ** This value is written to the Frame Type field in the actual Xbee frame.
 **
 ** @{
 **/

/** Default value defined simply for unrecognized frame types. This should not be sent to Xbee! */
#define FT_DEFAULT_FRAMETYPE    0x00
#define FT_TRANSMIT_REQUEST     0x10    /**< Transmit Request */
#define FT_TRANSMIT_STATUS      0x8B    /**< Transmit Status */

/**
 ** @}
 **/

/** **************************************************
 ** @defgroup PAYLOAD_OFFSETS Payload Offsets
 **
 ** @brief Payload Offset is the offset into the Xbee frame that points to the start of the payload.
 **
 ** @{
 **/

#define PO_TRANSMIT_REQUEST 17  /**< Transmit Request Payload Offset */

/**
 ** @}
 **/

/** **************************************************
 ** @defgroup FRAME_LEN_OFFSETS Frame Length Offsets
 **
 ** Frame Length Offset is the fixed-length offset to add
 ** to the size of the payload to get the value of
 ** the Length field in the Xbee frame.
 **
 ** FLO + PS = LEN
 **
 ** where:
 **   FLO - Frame Length Offset
 **   PS - Payload size
 **   LEN - Length field in Xbee frame
 **
 ** @{
 **/
#define FLO_TRANSMIT_REQUEST 14

/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup DELIVERY_STATUS TransmitStatus Frame - DeliveryStatus Field
 *
 * The TXS_DELIVSTAT_X definitions are to be used to interpret the Delivery Status
 * field of the Xbee Transmit Status Frame
 *
 * @{
 */

#define TXS_DELIVSTAT_SUCCESS       0x00    /**< Success */
#define TXS_DELIVSTAT_MACFAIL       0x01    /**< MAC ACK Failure */
#define TXS_DELIVSTAT_INV_END       0x15    /**< Invalid destination endpoint */
#define TXS_DELIVSTAT_NETFAIL       0x21    /**< Network ACK Failure */
#define TXS_DELIVSTAT_RT_NFND       0x25    /**< Route not found */
#define TXS_DELIVSTAT_PLD2BIG       0x74    /**< Data payload too large */

/**
 ** @}
 **/

/** ***********************************************************************
 ** @defgroup DISCOVERY_STATUS TransmitStatus Frame - DiscoveryStatus Field
 **
 ** @{
 **/

#define TXS_DISCVSTAT_NDOVHD        0x00    // No discovery overhead
#define TXS_DISCVSTAT_RTDSCV        0x02    // Route discovery

/**
 ** @}
 **/

#endif // XBEE_API_FRAME_DEFINES_H
