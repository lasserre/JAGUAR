// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/// @file	GCS_MAVLink.cpp

/*
This provides some support code and variables for MAVLink enabled sketches

This firmware is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
*/

#include <AP_HAL.h>
#include <AP_Common.h>
#include <GCS_MAVLink.h>

#ifdef MAVLINK_SEPARATE_HELPERS
#include "include/mavlink/v1.0/mavlink_helpers.h"
#endif

#define XBEE_API_MODE  DISABLED

#if XBEE_API_MODE == ENABLED
#include <xbee_framewriter.h>
#include <xbee_frame_structs.h>
#endif // XBEE_API_MODE == ENABLED


AP_HAL::BetterStream	*mavlink_comm_0_port;
AP_HAL::BetterStream	*mavlink_comm_1_port;

mavlink_system_t mavlink_system = {7,1,0,0};

uint8_t mavlink_check_target(uint8_t sysid, uint8_t compid)
{
    if (sysid != mavlink_system.sysid)
        return 1;
    // Currently we are not checking for correct compid since APM is not passing mavlink info to any subsystem
    // If it is addressed to our system ID we assume it is for us
    return 0; // no error
}

// return a MAVLink variable type given a AP_Param type
uint8_t mav_var_type(enum ap_var_type t)
{
    if (t == AP_PARAM_INT8) {
	    return MAVLINK_TYPE_INT8_T;
    }
    if (t == AP_PARAM_INT16) {
	    return MAVLINK_TYPE_INT16_T;
    }
    if (t == AP_PARAM_INT32) {
	    return MAVLINK_TYPE_INT32_T;
    }
    // treat any others as float
    return MAVLINK_TYPE_FLOAT;
}


/*
  send a buffer out a MAVLink channel
 */
void comm_send_buffer(mavlink_channel_t chan, const uint8_t *buf, uint8_t len)
{
#if XBEE_API_MODE == ENABLED
    XbeeAPI::TxRequest options;
    XbeeAPI::XBFrame::FrameByteArray frame;
    uint16_t frameLen;

    options.DestAddress = BROADCAST_ADDRESS; //TODO: set this to the GCS address
    options.PayloadStart = buf;
    options.PayloadLength = len;
    frameLen = XbeeAPI::FrameWriter::CalcEntireFrameLength(XbeeAPI::XBFrame::TransmitRequest, len);
    frame.FrameStart = new uint8_t[frameLen];
    XbeeAPI::FrameWriter::WriteTxRequestFrame(options, frame);

    switch(chan) {
    case MAVLINK_COMM_0:
        mavlink_comm_0_port->write(frame.FrameStart, frameLen);
        break;
    case MAVLINK_COMM_1:
        mavlink_comm_1_port->write(frame.FrameStart, frameLen);
        break;
    default:
        break;
    }

    delete [] frame.FrameStart;
    frame.FrameStart = 0;
#else
    switch(chan) {
	case MAVLINK_COMM_0:
		mavlink_comm_0_port->write(buf, len);
		break;
	case MAVLINK_COMM_1:
		mavlink_comm_1_port->write(buf, len);
		break;
	default:
		break;
	}
#endif
}

static const uint8_t mavlink_message_crc_progmem[256] PROGMEM = MAVLINK_MESSAGE_CRCS;

// return CRC byte for a mavlink message ID
uint8_t mavlink_get_message_crc(uint8_t msgid)
{
	return pgm_read_byte(&mavlink_message_crc_progmem[msgid]);
}

extern const AP_HAL::HAL& hal;

/*
  return true if the MAVLink parser is idle, so there is no partly parsed
  MAVLink message being processed
 */
bool comm_is_idle(mavlink_channel_t chan)
{
	mavlink_status_t *status = mavlink_get_channel_status(chan);
	return status == NULL || status->parse_state <= MAVLINK_PARSE_STATE_IDLE;
}
