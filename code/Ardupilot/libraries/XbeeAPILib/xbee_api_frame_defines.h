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

// Globally-used values

#define RCV_OPTS_ACK            0x01    ///< Receive Options Field - Packet Acknowledged
#define RCV_OPTS_BCP            0x02    ///< Receive Options Field - Packet was a broadcast packet


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

// Basic frame values

#define TXR_FRAMETYPE_ENUM      0x10     ///< Value written to Frame Type field for TxRequest frames
#define TXR_BASEFRAME_SIZE      14       ///< Value of LEN field NOT including payload. (e.g. LEN = BASE_SIZE + PAYLOAD_SIZE)

/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup EXPLICIT_ADDR_COMMAND ExplicitAddressingCommand Frame Definitions
 **
 ** @brief The EXA_X definitions are to be used to interpret the fields of the Xbee Explicit Addressing Command Frame
 **
 ** @{
 **/

// Basic frame values

#define EXA_FRAMETYPE_ENUM      0x11    ///< Value written to Frame Type field for ExAddrCmd frames
#define EXA_BASEFRAME_SIZE      20      ///< Value of LEN field NOT including payload (if any)

#define EXA_MIN_LENVALUE        (EXA_BASEFRAME_SIZE)    ///< Min value of LENGTH field for ExAddrCmd frames
#define EXA_MAX_LENVALUE        (XB_MAX_PAYLOADSIZE)    ///< Max value of LENGTH field for ExAddrCmd frames
#define EXA_MIN_TOTALSIZE       (XB_BASEFRAME_SIZE + EXA_MIN_LENVALUE)  ///< Min total frame size for ExAddrCmd frames
#define EXA_MAX_TOTALSIZE       (XB_BASEFRAME_SIZE + EXA_MAX_LENVALUE)  ///< Max total frame size for ExAddrCmd frames

// Field-specific

#define EXA_FRAMEID_IDX         4       ///< Frame ID Field Index

#define EXA_DESTADDR_IDX        5       ///< Destination Address Field Index

#define EXA_RESERVED_IDX        13      ///< Reserved Field Index

#define EXA_SRCENDPT_IDX        15      ///< Source Endpoint Field Index

#define EXA_DSTENDPT_IDX        16      ///< Destination Endpoint Field Index

#define EXA_CLUSTERID_IDX       17      ///< Cluster ID Field Index

#define EXA_PROFILEID_IDX       19      ///< Profile ID Field Index

#define EXA_BCSTRAD_IDX         21      ///< Broadcast Radius Field Index

#define EXA_TXOPTIONS_IDX       22      ///< Transmit Options Field Index
#define EXA_TXOPTIONS_NONE      0x00    ///< Transmit Options Field - No options specified
#define EXA_TXOPTIONS_DACK      0x01    ///< Transmit Options Field - Disable ACK
#define EXA_TXOPTIONS_NDSC      0x02    ///< Transmit Options Field - Don't attempt route discovery
#define EXA_TXOPTIONS_BOTH      0x03    ///< Transmit Options Field - Disable ACK & Don't attempt route discovery

#define EXA_PAYLOAD_IDX         23      ///< Payload Field Index

/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup LINK_TEST_REQUEST LinkTestRequest (Explicit Addressing Frame) Definitions
 **
 ** @brief The LTQ_X definitions are to be used to interpret the fields of the Xbee Explicit Addressing Command Frame
 ** for the specific case of a link test request packet
 **
 ** @{
 **/

// Link test generic

#define LT_SRCENDPT_VAL         0xe6    ///< Value of src endpoint in link test
#define LT_DSTENDPT_VAL         0xe6    ///< Value of dst endpoint in link test

// Link test request-specific

#define LTQ_PAYLOAD_SIZE        12      ///< Length of Link Test Request payload

#define LTQ_DESTADDR_PLDIDX     0       ///< Destination Address Field Index (w.r.t. the payload of ExAddCmd frame)
#define LTQ_PLDSIZE_PLDIDX      8       ///< Payload Size Field Index (w.r.t. the payload of ExAddCmd frame)
#define LTQ_NUMITER_PLDIDX      10      ///< Number of Iterations Field Index (w.r.t. the payload of ExAddCmd frame)


/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup EX_RX_INDICATOR Explicit Rx Indicator Frame Definitions
 **
 ** @brief The ERI_X definitions are to be used to interpret the fields of the Xbee Explicit Rx Indicator Frame
 **
 ** @{
 **/

// Basic frame values

#define ERI_FRAMETYPE_ENUM      0x91    ///< Value written to FRAMETYPE field for ExRxIndicator frames
#define ERI_BASEFRAME_SIZE      18      ///< Value of LEN field NOT including payload (if any)

#define ERI_MIN_LENVALUE        (ERI_BASEFRAME_SIZE)    ///< Min value of LEN field for ExRxIndicator frames
#define ERI_MAX_LENVALUE        (XB_MAX_PAYLOADSIZE)    ///< Max value of LEN field for ExRxIndicator frames
#define ERI_MIN_TOTALSIZE       (XB_BASEFRAME_SIZE + ERI_MIN_LENVALUE)  ///< Min total frame size for ExRxIndicator frames
#define ERI_MAX_TOTALSIZE       (XB_BASEFRAME_SIZE + ERI_MAX_LENVALUE)  ///< Max total frame size for ExRxIndicator frames

// Field-specific

#define ERI_SOURCEADDR_IDX      4       ///< Source Address Field Index

#define ERI_RESERVED_IDX        12      ///< Reserved Field Index

#define ERI_SRCENDPT_IDX        14      ///< Source Endpoint Field Index

#define ERI_DSTENDPT_IDX        15      ///< Destination Endpoint Field Index

#define ERI_CLUSTERID_IDX       16      ///< Cluster ID Field Index

#define ERI_PROFILEID_IDX       18      ///< Profile ID Field Index

#define ERI_RCVOPTS_IDX         20      ///< Receive Options Field Index

#define ERI_PAYLOAD_IDX         21      ///< Payload Field Index

/**
 ** @}
 **/

/** *********************************************************************
 ** @defgroup LINK_TEST_RESULT LinkTestResult (Explicit Addressing Frame) Definitions
 **
 ** @brief The LTR_X definitions are to be used to interpret the fields of the Xbee Explicit Addressing Command Frame
 ** for the specific case of a link test request packet
 **
 ** @{
 **/

#define LTR_PAYLOAD_SIZE        21      ///< Size of Link Test Result payload

#define LTR_CLUSTERID_VAL       0x0094  ///< Value of Cluster ID return in Link Test Result

#define LTR_DESTADDR_PLDIDX     0       ///< Destination address Field Index (wrt payload)
#define LTR_PLDSIZE_PLDIDX      8       ///< Payload size Field Index "
#define LTR_NUMITER_PLDIDX      10      ///< Number of iterations Field Index "
#define LTR_SUCCESS_PLDIDX      12      ///< Success Field Index "
#define LTR_RETRIES_PLDIDX      14      ///< Retries Field Index "
#define LTR_RESULT_PLDIDX       16      ///< Result Field Index "
#define LTR_MAXRETR_PLDIDX      17      ///< RR (max MAC retries) Field Index "
#define LTR_MAXRSSI_PLDIDX      18      ///< maxRSSI Field Index "
#define LTR_MINRSSI_PLDIDX      19      ///< minRSSI Field Index "
#define LTR_AVGRSSI_PLDIDX      20      ///< avgRSSI Field Index "

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

// Basic frame values

#define TXS_FRAMETYPE_ENUM      0x8B    ///< Value written to FRAMETYPE field for TxStatus frames
#define TXS_BASEFRAME_SIZE      7       ///< Value of LEN field NOT including payload (if any)

#define TXS_MIN_LENVALUE        (TXS_BASEFRAME_SIZE)    ///< Minimum value of LENGTH field for TxStatus frames
#define TXS_MAX_LENVALUE        (TXS_BASEFRAME_SIZE)    ///< Maximum value of LENGTH field for TxStatus frames
#define TXS_MIN_TOTALSIZE       (XB_BASEFRAME_SIZE + TXS_MIN_LENVALUE)    ///< Minimum total frame size for TxStatus frames
#define TXS_MAX_TOTALSIZE       (XB_BASEFRAME_SIZE + TXS_MAX_LENVALUE)    ///< Maximum total frame size for TxStatus frames

// Field-specific

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

#define RXP_PAYLOAD_IDX         15      ///< Payload Field Index

/**
 ** @}
 **/

#endif // XBEE_API_FRAME_DEFINES_H
