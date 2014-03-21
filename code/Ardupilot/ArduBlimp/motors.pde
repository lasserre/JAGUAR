/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#define THRUST_ARMING_TOLERANCE     20
#define ANTI_LIFT_ARMING_TOLERANCE  40
#define PITCH_ARMING_TOLERANCE      10
#define YAW_ARMING_TOLERANCE        10

/**
 * @brief ensure RC inputs are positioned for motors to be off
 * @return true if RC inputs are positioned for motors to be off
 */
bool check_rc_values()
{
    // ensure thrust control is at its minimum value
    if (g.rc_3.radio_in > g.rc_3.radio_min + THRUST_ARMING_TOLERANCE)
    {
        return false;
    }

    // ensure anti-lift control is at its minimum value
    if (g.rc_6.radio_in > g.rc_6.radio_min + ANTI_LIFT_ARMING_TOLERANCE)
    {
        return false;
    }

    // ensure pitch control stick is at its mid value
    int16_t pitch_radio_mid = (g.rc_2.radio_min + g.rc_2.radio_max) / 2;
    if (g.rc_2.radio_in < pitch_radio_mid - PITCH_ARMING_TOLERANCE || g.rc_2.radio_in > pitch_radio_mid + PITCH_ARMING_TOLERANCE)
    {
        return false;
    }

    // ensure yaw control stick is at its mid value
    int16_t yaw_radio_mid = (g.rc_4.radio_min + g.rc_4.radio_max) / 2;
    if (g.rc_4.radio_in < yaw_radio_mid - YAW_ARMING_TOLERANCE || g.rc_4.radio_in > yaw_radio_mid + YAW_ARMING_TOLERANCE)
    {
        return false;
    }

    return true;
}

// arm_motors_check - checks for pilot input to arm or disarm the blimp
// called at 10hz
static void arm_motors_check()
{
    static bool armingError = false;

    // ensure we are in Stabilize or Acro mode
    if (control_mode != ACRO && control_mode != STABILIZE)
    {
        return;
    }

    int16_t radio_mid = (g.rc_7.radio_min + g.rc_7.radio_max) / 2;

    // check if switch is in the armed position
    if (g.rc_7.radio_in > radio_mid)
    {
        if (!motors.armed())
        {
            if (!armingError)
            {
                // run pre-arm-checks and display failures
                pre_arm_checks(true);
                if (ap.pre_arm_check && check_rc_values())
                {
                    init_arm_motors();
                }
                else
                {
                    // set arming error flag if pre-arm checks fail
                    armingError = true;
                }
            }
        }
    }
    else // switch is in the disarm position
    {
        armingError = false; // reset arming error flag

        if (motors.armed())
        {
            // disarm the motors
            init_disarm_motors();
        }
    }
}

// TODO: Do we need this? It should be refactored if we do. We don't want the blimp
// to automatically disarm if the thrust motors are off for 25 sec - JBW
#if 0
// auto_disarm_check - disarms the blimp if it has been sitting on the ground in manual mode with throttle low for at least 25 seconds
// called at 1hz
static void auto_disarm_check()
{
    static uint8_t auto_disarming_counter;

    // if((control_mode <= ACRO) && (g.rc_3.control_in == 0) && motors.armed()) {
    if((control_mode == STABILIZE) && (g.rc_3.control_in == 0) && motors.armed()) {
        auto_disarming_counter++;

        if(auto_disarming_counter == AUTO_DISARMING_DELAY) {
            init_disarm_motors();
        }else if (auto_disarming_counter > AUTO_DISARMING_DELAY) {
            auto_disarming_counter = AUTO_DISARMING_DELAY + 1;
        }
    }else{
        auto_disarming_counter = 0;
    }
}
#endif // #if 0

// init_arm_motors - performs arming process including initialisation of barometer and gyros
static void init_arm_motors()
{
	// arming marker
    // Flag used to track if we have armed the motors the first time.
    // This is used to decide if we should run the ground_start routine
    // which calibrates the IMU
    static bool did_ground_start = false;

    // disable cpu failsafe because initialising everything takes a while
    failsafe_disable();

#if LOGGING_ENABLED == ENABLED
    // start dataflash
    start_logging();
#endif

#if HIL_MODE != HIL_MODE_DISABLED || CONFIG_HAL_BOARD == HAL_BOARD_AVR_SITL
    gcs_send_text_P(SEVERITY_HIGH, PSTR("ARMING MOTORS"));
#endif

    // we don't want writes to the serial port to cause us to pause
    // mid-flight, so set the serial ports non-blocking once we arm
    // the motors
    hal.uartA->set_blocking_writes(false);
#if 0 //TODO:remove
    if (gcs3.initialised) {
        hal.uartC->set_blocking_writes(false);
    }
#endif // #if 0

#if BLIMP_LEDS == ENABLED
    piezo_beep_twice();
#endif

    // Remember Orientation
    // --------------------
    initial_armed_bearing = ahrs.yaw_sensor;

    // Reset home position
    // -------------------
    if(ap.home_is_set)
        init_home();

    // all I terms are invalid
    // -----------------------
    reset_I_all();

    if(did_ground_start == false) {
        did_ground_start = true;
        startup_ground();
    }

#if HIL_MODE != HIL_MODE_ATTITUDE
    // read Baro pressure at ground -
    // this resets Baro for more accuracy
    //-----------------------------------
    init_barometer();
#endif

    // go back to normal AHRS gains
    ahrs.set_fast_gains(false);
#if SECONDARY_DMP_ENABLED == ENABLED
    ahrs2.set_fast_gains(false);
#endif

    // enable gps velocity based centrefugal force compensation
    ahrs.set_correct_centrifugal(true);

    // set hover throttle
    motors.set_mid_throttle(g.throttle_mid);

    // update leds on board
    update_arming_light();

#if BLIMP_LEDS == ENABLED
    piezo_beep_twice();
#endif

    // enable output to motors
    output_min();

    // finally actually arm the motors
    motors.armed(true);

#if LOGGING_ENABLED == ENABLED
    // log arming to dataflash
    Log_Write_Event(DATA_ARMED);
#endif // LOGGING_ENABLED

    // reenable failsafe
    failsafe_enable();
}

// perform pre-arm checks and set ap.pre_arm_check flag
static void pre_arm_checks(bool display_failure)
{
    // exit immediately if we've already successfully performed the pre-arm check
    if( ap.pre_arm_check ) {
        return;
    }

    // succeed if pre arm checks are disabled
    if(!g.arming_check_enabled) {
        ap.pre_arm_check = true;
        return;
    }

    // pre-arm rc checks a prerequisite
    pre_arm_rc_checks();
    if(!ap.pre_arm_rc_check) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: RC not calibrated"));
#endif // #if 0
        }
        return;
    }

// TODO: Do we want to check these? If the blimp gets disarmed in mid-air
// and needs to be re-armed, we don't care about the following - JBW
#if 0
    // check accelerometers have been calibrated
    if(!ins.calibrated()) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: INS not calibrated"));
#endif // #if 0
        }
        return;
    }

    // check the compass is healthy
    if(!compass.healthy) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: Compass not healthy"));
#endif // #if 0
        }
        return;
    }

    // check compass learning is on or offsets have been set
    Vector3f offsets = compass.get_offsets();
    if(!compass._learn && offsets.length() == 0) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: Compass not calibrated"));
#endif // #if 0
        }
        return;
    }

    // check for unreasonable compass offsets
    if(offsets.length() > 500) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: Compass offsets too high"));
#endif // #if 0
        }
        return;
    }

    // check for unreasonable mag field length
    float mag_field = pythagorous3(compass.mag_x, compass.mag_y, compass.mag_z);
    if (mag_field > COMPASS_MAGFIELD_EXPECTED*1.65 || mag_field < COMPASS_MAGFIELD_EXPECTED*0.35) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: Check mag field"));
#endif // #if 0
        }
        return;
    }

    // barometer health check
    if(!barometer.healthy) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: Baro not healthy"));
#endif // #if 0
        }
        return;
    }

#if AC_FENCE == ENABLED
    // check fence is initialised
    if(!fence.pre_arm_check()) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: No GPS Lock"));
#endif // #if 0
        }
        return;
    }
#endif

#if CONFIG_HAL_BOARD != HAL_BOARD_PX4
    // check board voltage
    if(board_voltage() < BOARD_VOLTAGE_MIN || board_voltage() > BOARD_VOLTAGE_MAX) {
        if (display_failure) {
#if 0 //TODO: enable when enabling GCS code - JBW
            gcs_send_text_P(SEVERITY_HIGH,PSTR("PreArm: Check Board Voltage"));
#endif // #if 0
        }
        return;
    }
#endif
#endif // #if 0

    // if we've gotten this far then pre arm checks have completed
    ap.pre_arm_check = true;
}

// perform pre_arm_rc_checks checks and set ap.pre_arm_rc_check flag
static void pre_arm_rc_checks()
{
    // exit immediately if we've already successfully performed the pre-arm rc check
    if( ap.pre_arm_rc_check ) {
        return;
    }

    // check if radio has been calibrated
    if(!g.rc_3.radio_min.load()) {
        return;
    }

    // check if throttle min is reasonable
    if(g.rc_3.radio_min > 1300) {
        return;
    }

    // if we've gotten this far rc is ok
    ap.pre_arm_rc_check = true;
}

static void init_disarm_motors()
{
#if HIL_MODE != HIL_MODE_DISABLED || CONFIG_HAL_BOARD == HAL_BOARD_AVR_SITL
    gcs_send_text_P(SEVERITY_HIGH, PSTR("DISARMING MOTORS"));
#endif

    motors.armed(false);

#if 0 //TODO: enable if needed - JBW
    compass.save_offsets();

    g.throttle_cruise.save();

    // we are not in the air
    set_takeoff_complete(false);
#endif // #if 0

#if BLIMP_LEDS == ENABLED
    piezo_beep();
#endif

#if 0 //TODO: enable if needed - JBW
    // setup fast AHRS gains to get right attitude
    ahrs.set_fast_gains(true);
#if SECONDARY_DMP_ENABLED == ENABLED
    ahrs2.set_fast_gains(true);
#endif
#endif // #if 0

#if LOGGING_ENABLED == ENABLED
    // log disarm to the dataflash
    Log_Write_Event(DATA_DISARMED);
#endif // LOGGING_ENABLED

#if 0 //TODO: enable if needed - JBW
    // disable gps velocity based centrefugal force compensation
    ahrs.set_correct_centrifugal(false);
#endif // #if 0
}
