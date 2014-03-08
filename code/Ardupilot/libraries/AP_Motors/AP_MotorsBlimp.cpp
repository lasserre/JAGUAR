// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
 *       AP_MotorsBlimp.cpp - ArduBlimp motors library
 *       Code by Wesley Eledui, Kyle Johnson, Caleb Stewart, John Wilkes
 */

#include "AP_MotorsBlimp.h"
#include <AP_HAL.h>

extern const AP_HAL::HAL& hal;

AP_MotorsBlimp::AP_MotorsBlimp(RC_Channel* rc_roll, RC_Channel* rc_pitch, RC_Channel* rc_throttle, RC_Channel* rc_yaw, RC_Channel* rc_lift, uint16_t speed_hz) :
  AP_Motors(rc_roll, rc_pitch, rc_throttle, rc_yaw, speed_hz),
  _rc_lift(rc_lift)
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

/**
 * @brief [brief description]
 * @details [long description]
 */
void AP_MotorsBlimp::output_armed()
{
    _rc_throttle->calc_pwm();
    _rc_lift->calc_pwm();
    _rc_roll->calc_pwm();
    _rc_pitch->calc_pwm();
    _rc_yaw->calc_pwm();

    // lift variables
    const int16_t& lift_radio_out = _rc_lift->radio_out;
    const int16_t& lift_radio_min = _rc_lift->radio_min;
    const int16_t& lift_radio_max = _rc_lift->radio_max;
    int16_t lift_radio_mid = (lift_radio_min + lift_radio_max) / 2;

    // roll variables
    const int16_t& roll_radio_out = _rc_roll->radio_out;
    const int16_t& roll_radio_min = _rc_roll->radio_min;
    const int16_t& roll_radio_max = _rc_roll->radio_max;
    int16_t roll_radio_mid = (roll_radio_min + roll_radio_max) / 2;

    // pitch variables
    const int16_t& pitch_radio_out = _rc_pitch->radio_out;
    const int16_t& pitch_radio_min = _rc_pitch->radio_min;
    const int16_t& pitch_radio_max = _rc_pitch->radio_max;
    int16_t pitch_radio_mid = (pitch_radio_min + pitch_radio_max) / 2;

    // yaw variables
    const int16_t& yaw_radio_out = _rc_yaw->radio_out;
    const int16_t& yaw_radio_min = _rc_yaw->radio_min;  // Control stick is pushed to the right
    const int16_t& yaw_radio_max = _rc_yaw->radio_max;  // Control stick is pushed to the left
    int16_t yaw_radio_mid = (yaw_radio_min + yaw_radio_max) / 2;    // Control stick is neutral position

    /* ---------- THRUST MOTORS ---------- */
    motor_out[THRUST_MOTOR] = _rc_throttle->radio_out;

    /* ---------- LIFT MOTOR ---------- */
    if (lift_radio_out > lift_radio_mid + STICK_PAD)
    {
        // convert the amount the stick is above mid position to
        // the full motor speed range
        motor_out[LIFT_MOTOR] = ( ((long)(lift_radio_out - lift_radio_mid) * (long)(lift_radio_max - lift_radio_min)) / (lift_radio_max - lift_radio_mid) ) + lift_radio_min;
    }
    else
    {
        motor_out[LIFT_MOTOR] = lift_radio_min;
    }

    /* ---------- ANTI-LIFT MOTORS ---------- */
    if (lift_radio_out < lift_radio_mid - STICK_PAD)
    {
        int16_t value = ( ((long)(lift_radio_mid - lift_radio_out) * (long)(lift_radio_max - lift_radio_min)) / (lift_radio_mid - lift_radio_min) ) + lift_radio_min;
        motor_out[LEFT_ANTI_LIFT_MOTOR] = value;
        motor_out[RIGHT_ANTI_LIFT_MOTOR] = value;
    }
    else
    {
        motor_out[LEFT_ANTI_LIFT_MOTOR] = lift_radio_min;
        motor_out[RIGHT_ANTI_LIFT_MOTOR] = lift_radio_min;
    }

    /* ---------- YAW MOTORS ---------- */
    if (yaw_radio_out > yaw_radio_mid + STICK_PAD)
    {
        // Control stick in the RIGHT region (Right ON, Left off)
        long yawStickOffset = yaw_radio_out - yaw_radio_mid;    // Offset from center pos

        motor_out[LEFT_YAW_MOTOR] = yaw_radio_min;
        motor_out[RIGHT_YAW_MOTOR] = (yawStickOffset * 2) + yaw_radio_min;
    }
    else if (yaw_radio_out < yaw_radio_mid - STICK_PAD)
    {
        // Control stick in the LEFT region (Right off, Left ON)

        long yawStickOffset = yaw_radio_mid - yaw_radio_out;    // Offset from center pos

        motor_out[LEFT_YAW_MOTOR] = (yawStickOffset * 2) + yaw_radio_min;;
        motor_out[RIGHT_YAW_MOTOR] = yaw_radio_min;
    }
    else
    {
        // Control stick in the NEUTRAL region (Right off, Left off)
        motor_out[LEFT_YAW_MOTOR] = yaw_radio_min;
        motor_out[RIGHT_YAW_MOTOR] = yaw_radio_min;
    }

    /* ---------- PITCH MOTORS ---------- */
    if (pitch_radio_out > pitch_radio_mid + STICK_PAD)
    {
        // Control stick in the UPPER region - we want to pitch DOWN 

        long pitchStickOffset = pitch_radio_out - pitch_radio_mid;

        motor_out[PITCH_UP_MOTOR] = pitch_radio_min;
        motor_out[PITCH_DOWN_MOTOR] = (pitchStickOffset * 2) + pitch_radio_min;
    }
    else if (pitch_radio_out < pitch_radio_mid - STICK_PAD)
    {
        // Control stick in the LOWER region - we want to pitch UP

        long pitchStickOffset = pitch_radio_mid - pitch_radio_out;

        motor_out[PITCH_UP_MOTOR] = (pitchStickOffset * 2) + pitch_radio_min;
        motor_out[PITCH_DOWN_MOTOR] = pitch_radio_min;
    }
    else
    {
        // Control stick in the NEUTRAL region (Pitch motors off)
        motor_out[PITCH_UP_MOTOR] = pitch_radio_min;
        motor_out[PITCH_DOWN_MOTOR] = pitch_radio_min;
    }

    //TODO: update _reached_limit

    // send output to each motor
    for (uint8_t i = 0; i < NUM_BLIMP_MOTORS; ++i)
    {
        hal.rcout->write(_motor_to_channel_map[i], motor_out[i]);
    }
}

void AP_MotorsBlimp::output_disarmed()
{
    // send minimum values to all motors
    output_min();
}
