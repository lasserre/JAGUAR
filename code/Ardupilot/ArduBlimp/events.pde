// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/*
 *       This event will be called when the failsafe changes
 *       boolean failsafe reflects the current state
 */
static void failsafe_radio_on_event()
{
    // if motors are not armed there is nothing to do
    if( !motors.armed() ) {
        return;
    }

    // This is how to handle a failsafe.
    switch(control_mode) {
        case STABILIZE:
        case ACRO:
            // if throttle is zero disarm motors
            if (g.rc_3.control_in == 0) {
                init_disarm_motors();
            }
#if 0 //TODO:enable ACRO, RTL, and LAND modes
            else if(ap.home_is_set == true && g_gps->status() == GPS::GPS_OK_FIX_3D && home_distance > wp_nav.get_waypoint_radius()) {
                set_mode(RTL);
            }
            else{
                // We have no GPS or are very close to home so we will land
                set_mode(LAND);
            }
#endif // #if 0
            break;
#if 0 //TODO:remove
        case AUTO:
            // failsafe_throttle is 1 do RTL, 2 means continue with the mission
            if (g.failsafe_throttle == FS_THR_ENABLED_ALWAYS_RTL) {
                if(home_distance > wp_nav.get_waypoint_radius()) {
                    set_mode(RTL);
                }else{
                    // We are very close to home so we will land
                    set_mode(LAND);
                }
            }
            // if failsafe_throttle is 2 (i.e. FS_THR_ENABLED_CONTINUE_MISSION) no need to do anything
            break;
#endif // #if 0
        default:
#if 0 //TODO: enable ACRO, RTL, and LAND modes
            if(ap.home_is_set == true && g_gps->status() == GPS::GPS_OK_FIX_3D && home_distance > wp_nav.get_waypoint_radius()) {
                set_mode(RTL);
            }else{
                // We have no GPS or are very close to home so we will land
                set_mode(LAND);
            }
#endif // #if 0
            break;
    }

#if LOGGING_ENABLED == ENABLED
    // log the error to the dataflash
    Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_RADIO, ERROR_CODE_FAILSAFE_OCCURRED);
#endif // LOGGING_ENABLED

}

// failsafe_off_event - respond to radio contact being regained
// we must be in AUTO, LAND or RTL modes
// or Stabilize or ACRO mode but with motors disarmed
static void failsafe_radio_off_event()
{
    // no need to do anything except log the error as resolved
    // user can now override roll, pitch, yaw and throttle and even use flight mode switch to restore previous flight mode
#if LOGGING_ENABLED == ENABLED
    Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_RADIO, ERROR_CODE_FAILSAFE_RESOLVED);
#endif // LOGGING_ENABLED
}

static void low_battery_event(void)
{
    // failsafe check
    if (g.failsafe_battery_enabled && !ap.low_battery && motors.armed()) {
        switch(control_mode) {
            case STABILIZE:
            case ACRO:
                // if throttle is zero disarm motors
                if (g.rc_3.control_in == 0) {
                    init_disarm_motors();
                }
#if 0 //TODO: enable LAND mode
                else{
                    set_mode(LAND);
                }
#endif // #if 0
                break;
#if 0 //TODO:needed??
            case AUTO:
                if(ap.home_is_set == true && g_gps->status() == GPS::GPS_OK_FIX_3D && home_distance > wp_nav.get_waypoint_radius()) {
                    set_mode(RTL);
                }else{
                    // We have no GPS or are very close to home so we will land
                    set_mode(LAND);
                }
                break;
#endif // #if 0
            default:
#if 0 // TODO: enable LAND mode
                set_mode(LAND);
#endif // #if 0
                break;
        }
    }

    // set the low battery flag
    set_low_battery(true);

#if 0 //TODO:remove
    // warn the ground station and log to dataflash
    gcs_send_text_P(SEVERITY_LOW,PSTR("Low Battery!"));
#endif // #if 0
#if LOGGING_ENABLED == ENABLED
    Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_BATT, ERROR_CODE_FAILSAFE_OCCURRED);
#endif // LOGGING_ENABLED

#if BLIMP_LEDS == ENABLED
    // Only Activate if a battery is connected to avoid alarm on USB only
    piezo_on();
#endif // BLIMP_LEDS
}

// failsafe_gps_check - check for gps failsafe
static void failsafe_gps_check()
{
    uint32_t last_gps_update_ms;

    // return immediately if gps failsafe is disabled
    if( !g.failsafe_gps_enabled ) {
        return;
    }

    // calc time since last gps update
    last_gps_update_ms = millis() - g_gps->last_fix_time;

    // check if all is well
    if( last_gps_update_ms < FAILSAFE_GPS_TIMEOUT_MS) {
        // check for recovery from gps failsafe
        if( ap.failsafe_gps ) {
            failsafe_gps_off_event();
            set_failsafe_gps(false);
        }
        return;
    }

    // do nothing if gps failsafe already triggered or motors disarmed
    if( ap.failsafe_gps || !motors.armed()) {
        return;
    }

    // GPS failsafe event has occured
    // update state, warn the ground station and log to dataflash
    set_failsafe_gps(true);
#if 0 //TODO:remove
    gcs_send_text_P(SEVERITY_LOW,PSTR("Lost GPS!"));
#endif // #if 0
#if LOGGING_ENABLED == ENABLED
    Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_GPS, ERROR_CODE_FAILSAFE_OCCURRED);
#endif // LOGGING_ENABLED

#if 0 //TODO: enable LAND mode
    // take action based on flight mode
    if(mode_requires_GPS(control_mode))
        set_mode(LAND);

    // land if circular fence is enabled
    if((fence.get_enabled_fences() & AC_FENCE_TYPE_CIRCLE) != 0) {
        set_mode(LAND);
    }
#endif // #if 0
}

// failsafe_gps_off_event - actions to take when GPS contact is restored
static void failsafe_gps_off_event(void)
{
#if LOGGING_ENABLED == ENABLED
    // log recovery of GPS in logs?
    Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_GPS, ERROR_CODE_FAILSAFE_RESOLVED);
#endif // LOGGING_ENABLED
}

// failsafe_gcs_check - check for ground station failsafe
static void failsafe_gcs_check()
{
    uint32_t last_gcs_update_ms;

    // return immediately if gcs failsafe is disabled, gcs has never been connected or we are not overriding rc controls from the gcs
    if( g.failsafe_gcs == FS_GCS_DISABLED || last_heartbeat_ms == 0 || !ap.rc_override_active) {
        return;
    }

    // calc time since last gps update
    last_gcs_update_ms = millis() - last_heartbeat_ms;

    // check if all is well
    if( last_gcs_update_ms < FS_GCS_TIMEOUT_MS) {
        // check for recovery from gps failsafe
        if( ap.failsafe_gcs ) {
            failsafe_gcs_off_event();
            set_failsafe_gcs(false);
        }
        return;
    }

    // do nothing if gps failsafe already triggered or motors disarmed
    if( ap.failsafe_gcs || !motors.armed()) {
        return;
    }

    // GCS failsafe event has occured
    // update state, log to dataflash
    set_failsafe_gcs(true);
#if LOGGING_ENABLED == ENABLED
    Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_GCS, ERROR_CODE_FAILSAFE_OCCURRED);
#endif // LOGGING_ENABLED

    // This is how to handle a failsafe.
    // use the throttle failsafe setting to decide what to do
    switch(control_mode) {
        case STABILIZE:
#if 0 //TODO:enable
        case ACRO:
#endif // #if 0
            // if throttle is zero disarm motors
            if (g.rc_3.control_in == 0) {
                init_disarm_motors();
            }
#if 0 //TODO: enable ACRO, RTL, and LAND modes
            else if(ap.home_is_set == true && g_gps->status() == GPS::GPS_OK_FIX_3D && home_distance > wp_nav.get_waypoint_radius()) {
                set_mode(RTL);
            }
            else{
                // We have no GPS or are very close to home so we will land
                set_mode(LAND);
            }
#endif // #if 0
            break;
#if 0 //TODO:needed?
        case AUTO:
            // if g.failsafe_gcs is 1 do RTL, 2 means continue with the mission
            if (g.failsafe_gcs == FS_GCS_ENABLED_ALWAYS_RTL) {
                if(home_distance > wp_nav.get_waypoint_radius()) {
                    set_mode(RTL);
                }else{
                    // We are very close to home so we will land
                    set_mode(LAND);
                }
            }
            // if failsafe_throttle is 2 (i.e. FS_THR_ENABLED_CONTINUE_MISSION) no need to do anything
            break;
#endif // #if 0
        default:
#if 0 //TODO:enable ACRO, RTL, and LAND modes
            if(ap.home_is_set == true && g_gps->status() == GPS::GPS_OK_FIX_3D && home_distance > wp_nav.get_waypoint_radius()) {
                set_mode(RTL);
            }else{
                // We have no GPS or are very close to home so we will land
                set_mode(LAND);
            }
#endif // #if 0
            break;
    }
}

// failsafe_gps_off_event - actions to take when GPS contact is restored
static void failsafe_gcs_off_event(void)
{
#if LOGGING_ENABLED == ENABLED
    // log recovery of GPS in logs?
    Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_GCS, ERROR_CODE_FAILSAFE_RESOLVED);
#endif // LOGGING_ENABLED
}

static void update_events()     // Used for MAV_CMD_DO_REPEAT_SERVO and MAV_CMD_DO_REPEAT_RELAY
{
    if(event_repeat == 0 || (millis() - event_timer) < event_delay)
        return;

    if(event_repeat != 0) {             // event_repeat = -1 means repeat forever
        event_timer = millis();

        if (event_id >= CH_5 && event_id <= CH_8) {
            if(event_repeat%2) {
                hal.rcout->write(event_id, event_value);                 // send to Servos
            } else {
                hal.rcout->write(event_id, event_undo_value);
            }
        }

        if  (event_id == RELAY_TOGGLE) {
            relay.toggle();
        }
        if (event_repeat > 0) {
            event_repeat--;
        }
    }
}

