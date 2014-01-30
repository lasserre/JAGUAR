// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
 *       AP_MotorsBlimp.cpp - ArduBlimp motors library
 *       Code by Wesley Eledui.
 */

#include "AP_MotorsBlimp.h"

// setup_motors - configures the motors for a blmip
void AP_MotorsBlimp::setup_motors()
{
    // call parent
    AP_MotorsMatrix::setup_motors();

    // hard coded config for supported frames
    // When initializing motors, use AP_MOTORS_BLIMP_FRAME
    if( _frame_orientation == AP_MOTORS_BLIMP_FRAME ) {
    	
        //JAGUAR
    	//The way the angle (second parameter to the add_motor function)
        //is by the following equations:
    	//		cosine(angle+90) = amount that the motor helps 	roll
    	//		cosine(angle) = amount that the motor helps 	pitch
    	//		angle = amount that the motor helps				yaw

        //Propulsion motors
        //      This is actually two motors (tied together). The 3rd parameter
        //      is 0 because these motors do not affect yaw.
        add_motor(AP_MOTORS_MOT_1,  0, 0, 2);

        //Lift motor
        add_motor(AP_MOTORS_MOT_2, -90, 0, 4);


        add_motor(AP_MOTORS_MOT_3,   0, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  1);
        add_motor(AP_MOTORS_MOT_4, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_5, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_6, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_7, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_8, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);

    }
}
