// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
 *       AP_MotorsBlimp.cpp - ArduBlimp motors library
 *       Code by Wesley Eledui.
 */

#include "AP_MotorsBlimp.h"
#include <AP_HAL.h>

extern const AP_HAL::HAL& hal;

AP_MotorsBlimp::AP_MotorsBlimp(RC_Channel* rc_roll, RC_Channel* rc_pitch, RC_Channel* rc_throttle, RC_Channel* rc_yaw, uint16_t speed_hz) :
  AP_Motors(rc_roll, rc_pitch, rc_throttle, rc_yaw, speed_hz)
{
}

void AP_MotorsBlimp::set_update_rate(uint16_t speed_hz)
{
    // record requested speed
    AP_Motors::set_update_rate(speed_hz);

    uint32_t mask = 
        1U << _motor_to_channel_map[AP_MOTORS_MOT_1] |
        1U << _motor_to_channel_map[AP_MOTORS_MOT_2] |
        1U << _motor_to_channel_map[AP_MOTORS_MOT_3] |
        1U << _motor_to_channel_map[AP_MOTORS_MOT_4] |
        1U << _motor_to_channel_map[AP_MOTORS_MOT_5] |
        1U << _motor_to_channel_map[AP_MOTORS_MOT_6] |
        1U << _motor_to_channel_map[AP_MOTORS_MOT_7] |
        1U << _motor_to_channel_map[AP_MOTORS_MOT_8];
    hal.rcout->set_freq(mask, _speed_hz);
}

void AP_MotorsBlimp::enable()
{
    // enable output channels
    for (uint8_t i = 0; i < NUM_BLIMP_MOTORS; ++i)
    {
        hal.rcout->enable_ch(_motor_to_channel_map[i]);
    }
}

void AP_MotorsBlimp::output_min()
{
    // fill the motor_out[] array for HIL use and send minimum value to each motor
    for (int8_t i = 0; i < NUM_BLIMP_MOTORS; ++i)
    {
        motor_out[i] = _rc_throttle->radio_min;
        hal.rcout->write(_motor_to_channel_map[i], motor_out[i]);
    }
}

void AP_MotorsBlimp::output_test()
{
    // shut down all motors
    output_min();

    // first delay is longer
    hal.scheduler->delay(3000);

    // loop through all the possible orders spinning any motors that match that description
    for (uint8_t i = 0; i < NUM_BLIMP_MOTORS; ++i)
    {
        // turn on this motor and wait 1/3rd of a second
        hal.rcout->write(_motor_to_channel_map[i], _rc_throttle->radio_min + _min_throttle);
        hal.scheduler->delay(333);
        hal.rcout->write(_motor_to_channel_map[i], _rc_throttle->radio_min);
        hal.scheduler->delay(2000);
    }

    // shut down all motors
    output_min();
}

void AP_MotorsBlimp::output_armed()
{
    _rc_throttle->calc_pwm();
    hal.console->println(_rc_throttle->radio_in);


    if (_rc_throttle->radio_in > 1500) {
        hal.rcout->write(_motor_to_channel_map[3], _rc_throttle->radio_min + _min_throttle);
        if(_rc_throttle->radio_in > 1700)
            hal.rcout->write(_motor_to_channel_map[2], _rc_throttle->radio_min + _min_throttle);
    }
    else
    {
        hal.rcout->write(_motor_to_channel_map[3], _rc_throttle->radio_min);
        hal.rcout->write(_motor_to_channel_map[2], _rc_throttle->radio_min);
    }

    //TODO: add code for motor output when armed
}

void AP_MotorsBlimp::output_disarmed()
{
    // send minimum values to all motors
    output_min();
}
