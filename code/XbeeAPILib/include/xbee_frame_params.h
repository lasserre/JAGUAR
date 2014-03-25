#ifndef XBEE_FRAME_PARAMS_H
#define XBEE_FRAME_PARAMS_H

namespace XbeeAPI {

/**
 * @brief The TxRequestParams struct holds the modifiable fields of the Transmit Request Xbee frame.
 */
struct TxRequestParams
{
    uint64_t DestAddress;
    char* PayloadStart;
    uint16_t PayloadLength;
};

} // XbeeAPI

#endif // XBEE_FRAME_PARAMS_H
