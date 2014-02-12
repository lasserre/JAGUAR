// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
 *       AP_MotorsBlimp.cpp - ArduBlimp motors library
 *       Code by Wesley Eledui.
 */

#include "AP_MotorsBlimp.h"

// setup_motors - configures the motors for a blimp
void AP_MotorsBlimp::setup_motors()
{
    // call parent
    AP_MotorsMatrix::setup_motors();

    // hard coded config for supported frames
    // When initializing motors, use AP_MOTORS_BLIMP_FRAME
    if( _frame_orientation == AP_MOTORS_BLIMP_FRAME ) {
    	
        //JAGUAR
    	//The way the add_motor parameters affect roll pitch and yaw
    	//		cosine(2ndparam+90) = amount that the motor helps      roll
    	//		cosine(2ndparam) = amount that the motor helps         pitch
    	//		3rdparam = amount that the motor helps                 yaw

        //Propulsion motors
        //      This is actually two motors (tied together). The 3rd parameter
        //      is 0 because these motors do not affect yaw.
        add_motor(THRUST_MOTOR,  0, 0, 1);

        //Lift motor
        add_motor(LIFT_MOTOR, -90, 0, 2);

        //Weignht compensation motors
        add_motor(LEFT_ANTI_LIFT_MOTOR,     0, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(RIGHT_ANTI_LEFT_MOTOR,    0, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  4);

        //Yaw motors
        add_motor_raw(LEFT_YAW_MOTOR,		0, 0, 1, 5);
        add_motor_raw(RIGHT_YAW_MOTOR,		0, 0, 1, 6);

        //Pitch motors
        add_motor_raw(TOP_PITCH_MOTOR, 		0, 1, 0, 7);
        add_motor_raw(TOP_PITCH_MOTOR, 		0, 1, 0, 8);

    }
}
