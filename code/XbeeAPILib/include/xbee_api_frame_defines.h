#ifndef XBEE_API_FRAME_DEFINES_H
#define XBEE_API_FRAME_DEFINES_H

/** @file */

/** **************************************************
 ** @defgroup XB_GENERIC_DEFS Generic Xbee Frame Definitions
 **
 ** @brief The Xbee-generic definitions describe the base layer of an Xbee frame.
 **
 ** The Xbee-generic layer encapsulates frame-specific data for every frame type. The format is:
 **
 ** [ Xbee-generic header ] [ Frame-specific data ... ] [CHECKSUM]
 **
 ** or specifically...
 **
 ** [START] [LEN (1)] [LEN (2)] [FRAMETYPE] [ Frame-specific data ... ] [CHECKSUM]
 **
 ** @{
 **/

#define XB_HEADER_SIZE          3       ///< START byte + 2 LENGTH bytes
#define XB_CHECKSUM_SIZE        1       ///< Checksum size
#define XB_BASEFRAME_SIZE       (XB_HEADER_SIZE + XB_CHECKSUM_SIZE)  ///< Xbee-generic header + CHECKSUM byte

#define XB_START_IDX            0       ///< START byte Index
#define XB_START_VAL            0x7e    ///< START byte value

#define XB_LENMSB_IDX           1       ///< MSB LENGTH byte Index
#define XB_LENLSB_IDX           2       ///< LSB LENGTH byte Index
#define XB_FRAMETYPE_IDX        3       ///< FRAMETYPE byte Index

#define XB_MIN_PAYLOADSIZE      0       ///< Min size of payload (for frames that have a payload)
#define XB_MAX_PAYLOADSIZE      65536   ///< Max size of payload (for frames that have a payload) CLS: a little large, yes?

/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup TRANSMIT_REQUEST TransmitRequest Frame Definitions
 *
 * @brief The TXR_X definitions are to be used to interpret the fields of the Xbee Transmit Request Frame
 *
 * @{
 */

#define TXR_FRAMETYPE_ENUM      0x10     ///< Value written to Frame Type field for TxRequest frames
#define TXR_BASEFRAME_SIZE      14       ///< Value of LEN field NOT including payload. (e.g. LEN = BASE_SIZE + PAYLOAD_SIZE)

/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup TRANSMIT_STATUS TransmitStatus Frame Definitions
 *
 * @brief The TXS_X definitions are to be used to interpret the fields of the Xbee Transmit Status Frame
 *
 * @{
 */

#define TXS_FRAMETYPE_ENUM      0x8B    ///< Value written to FRAMETYPE field for TxStatus frames
#define TXS_BASEFRAME_SIZE      7       ///< Value of LEN field NOT including payload (if any)

#define TXS_MIN_LENVALUE        (TXS_BASEFRAME_SIZE)    ///< Minimum value of LENGTH field for TxStatus frames
#define TXS_MAX_LENVALUE        (TXS_BASEFRAME_SIZE)    ///< Maximum value of LENGTH field for TxStatus frames
#define TXS_MIN_TOTALSIZE       (XB_BASEFRAME_SIZE + TXS_MIN_LENVALUE)    ///< Minimum total frame size for TxStatus frames
#define TXS_MAX_TOTALSIZE       (XB_BASEFRAME_SIZE + TXS_MAX_LENVALUE)    ///< Maximum total frame size for TxStatus frames

#define TXS_RTRYCOUNT_IDX       7       ///< Retry Count Field Index

#define TXS_DELIVSTAT_IDX       8       ///< Delivery Status Field Index
#define TXS_DELIVSTAT_SUCCESS   0x00    ///< Delivery Status Field - Success
#define TXS_DELIVSTAT_MACFAIL   0x01    ///< Delivery Status Field - MAC ACK Failure
#define TXS_DELIVSTAT_INV_END   0x15    ///< Delivery Status Field - Invalid destination endpoint
#define TXS_DELIVSTAT_NETFAIL   0x21    ///< Delivery Status Field - Network ACK Failure
#define TXS_DELIVSTAT_RT_NFND   0x25    ///< Delivery Status Field - Route not found
#define TXS_DELIVSTAT_PLD2BIG   0x74    ///< Delivery Status Field - Data payload too large

#define TXS_DISCVSTAT_IDX       9       ///< Discovery Status Field Index
#define TXS_DISCVSTAT_NDOVHD    0x00    ///< Discovery Status Field - No discovery overhead
#define TXS_DISCVSTAT_RTDSCV    0x02    ///< Discovery Status Field - Route discovery

/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup RECEIVE_PACKET ReceivePacket Frame Definitions
 **
 ** @brief The RXP_X definitions are used to interpret the fields of the Xbee Receive Packet Frame
 **
 ** @{
 **/

#define RXP_FRAMETYPE_ENUM      0x90    ///< Value written to FRAMETYPE field for RxPacket frames
#define RXP_BASEFRAME_SIZE      12      ///< Value of LEN field NOT including payload. (e.g. LEN = BASE_SIZE + PAYLOAD_SIZE)

#define RXP_MIN_LENVALUE        (RXP_BASEFRAME_SIZE + XB_MIN_PAYLOADSIZE)   ///< Min value of LENGTH field for RxPacket frames
#define RXP_MAX_LENVALUE        (RXP_BASEFRAME_SIZE + XB_MAX_PAYLOADSIZE)   ///< Max value of LENGTH field for RxPacket frames
#define RXP_MIN_TOTALSIZE       (XB_BASEFRAME_SIZE + RXP_MIN_LENVALUE)    ///< Min total frame size for RxPacket frames
#define RXP_MAX_TOTALSIZE       (XB_BASEFRAME_SIZE + RXP_MAX_LENVALUE)    ///< Max total frame size for RxPacket frames

#define RXP_SOURCEADDR_IDX        4     ///< Source Address Field Index

#define RXP_RESERVED_IDX        12      ///< Reserved Field Index

#define RXP_RCV_OPTS_IDX        14      ///< Receive Options Field Index
#define RXP_RCV_OPTS_ACK        0x01    ///< Receive Options Field - Packet Acknowledged
#define RXP_RCV_OPTS_BCP        0x02    ///< Receive Options Field - Packet was a broadcast packet

#define RXP_PAYLOAD_IDX         15      ///< Payload Field Index

/**
 ** @}
 **/

#endif // XBEE_API_FRAME_DEFINES_H
