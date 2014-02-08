// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/// @file	AP_MotorsBlimp.h
/// @brief	Motor control class for the JAGUAR Blimp

#ifndef __AP_MOTORS_BLIMP_H__
#define __AP_MOTORS_BLIMP_H__

#include <AP_Common.h>
#include <AP_Math.h>        // ArduPilot Mega Vector/Matrix math Library
#include <RC_Channel.h>     // RC Channel Library
#include "AP_MotorsMatrix.h"    // Parent Motors Matrix library

/// @class      AP_MotorsBlimp
class AP_MotorsBlimp : public AP_MotorsMatrix {
public:

    /// Constructor
    AP_MotorsBlimp( RC_Channel* rc_roll, RC_Channel* rc_pitch, RC_Channel* rc_throttle, RC_Channel* rc_yaw, uint16_t speed_hz = AP_MOTORS_SPEED_DEFAULT) : AP_MotorsMatrix(rc_roll, rc_pitch, rc_throttle, rc_yaw, speed_hz) {
    };

    // setup_motors - configures the motors for a blimp
    virtual void        setup_motors();

protected:

};

#endif  // __AP_MOTORS_BLIMP_H__
