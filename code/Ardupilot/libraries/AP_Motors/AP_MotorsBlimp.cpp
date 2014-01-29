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
    if( _frame_orientation == AP_MOTORS_JAGUAR_FRAME ) {
    	//JAGUAR 

    	//The way the degrees right here works is by the following equation
    	//		cosine(angle+90) = amount that the motor helps 	roll
    	//		cosine(angle) = amount that the motor helps 	pitch
    	//		angle = amount that the motor helps				yaw
    	
        add_motor(AP_MOTORS_MOT_1,  90, AP_MOTORS_MATRIX_YAW_FACTOR_CCW, 2);
        add_motor(AP_MOTORS_MOT_2, -90, AP_MOTORS_MATRIX_YAW_FACTOR_CCW, 4);
        add_motor(AP_MOTORS_MOT_3,   0, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  1);
        add_motor(AP_MOTORS_MOT_4, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_5, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_6, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_7, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        add_motor(AP_MOTORS_MOT_8, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);
        //NOTE: this will not compile because the maximum number of motors
        //allowed is only 8... Find a way around this...
        add_motor(AP_MOTORS_MOT_9, 180, AP_MOTORS_MATRIX_YAW_FACTOR_CW,  3);

    }
}
