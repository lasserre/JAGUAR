// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/// @file	AP_MotorsBlimp.h
/// @brief	Motor control class for the JAGUAR Blimp

#ifndef __AP_MOTORS_BLIMP_H__
#define __AP_MOTORS_BLIMP_H__

#include <AP_Common.h>
#include <AP_Math.h>        // ArduPilot Mega Vector/Matrix math Library
#include <RC_Channel.h>     // RC Channel Library
#include "AP_MotorsMatrix.h"    // Parent Motors Matrix library

// motor definitions
#define THRUST_MOTOR            AP_MOTORS_MOT_1  // this is actually 2 motors tied together
#define LIFT_MOTOR              AP_MOTORS_MOT_2
#define LEFT_ANTI_LIFT_MOTOR    AP_MOTORS_MOT_3
#define RIGHT_ANTI_LEFT_MOTOR   AP_MOTORS_MOT_4
#define LEFT_YAW_MOTOR	        AP_MOTORS_MOT_5
#define RIGHT_YAW_MOTOR         AP_MOTORS_MOT_6
#define TOP_PITCH_MOTOR         AP_MOTORS_MOT_7
#define BOTTOM_PITCH_MOTOR      AP_MOTORS_MOT_8

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
