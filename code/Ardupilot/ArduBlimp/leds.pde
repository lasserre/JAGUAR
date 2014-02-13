/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

// update_board_leds - updates leds on board
// should be called at 10hz
static void update_board_leds()
{
    // we need to slow down the calls to the GPS and dancing lights to 3.33hz
    static uint8_t counter = 0;
    if(++counter >= 3){
        counter = 0;
    }

    switch(led_mode) {
    case NORMAL_LEDS:
        update_arming_light();
        if (counter == 0) {
            update_GPS_light();
        }
        break;

    case SAVE_TRIM_LEDS:
        if (counter == 0) {
            dancing_light();
        }
        break;
    }
}

static void update_GPS_light(void)
{
    // GPS LED on if we have a fix or Blink GPS LED if we are receiving data
    // ---------------------------------------------------------------------
    switch (g_gps->status()) {
        case GPS::NO_FIX:
        case GPS::GPS_OK_FIX_2D:
            // check if we've blinked since the last gps update
            if (g_gps->valid_read) {
                g_gps->valid_read = false;
                ap_system.GPS_light = !ap_system.GPS_light;                     // Toggle light on and off to indicate gps messages being received, but no GPS fix lock
                if (ap_system.GPS_light) {
                    digitalWrite(C_LED_PIN, LED_OFF);
                }else{
                    digitalWrite(C_LED_PIN, LED_ON);
                }
            }
            break;

        case GPS::GPS_OK_FIX_3D:
            if(ap.home_is_set) {
                digitalWrite(C_LED_PIN, LED_ON);                  //Turn LED C on when gps has valid fix AND home is set.
            } else {
                digitalWrite(C_LED_PIN, LED_OFF);
            }
            break;

        default:
            digitalWrite(C_LED_PIN, LED_OFF);
            break;
    }
}

static void update_arming_light(void)
{
    // counter to control state
    static int8_t counter = 0;
    counter++;

    // disarmed
    if(!motors.armed()) {
        if(!ap.pre_arm_check) {
            // failed pre-arm checks so double flash
            switch(counter) {
                case 0:
                    ap_system.arming_light = true;
                    break;
                case 1:
                    ap_system.arming_light = false;
                    break;
                case 2:
                    ap_system.arming_light = true;
                    break;
                case 3:
                case 4:
                    ap_system.arming_light = false;
                    break;
                default:
                    // reset counter to restart the sequence
                    counter = -1;
                    break;
            }
        }else{
            // passed pre-arm checks so slower single flash
            switch(counter) {
                case 0:
                case 1:
                case 2:
                    ap_system.arming_light = true;
                    break;
                case 3:
                case 4:
                case 5:
                    ap_system.arming_light = false;
                    break;
                default:
                    // reset counter to restart the sequence
                    counter = -1;
                    break;
            }
        }
        // set arming led from arming_light flag
        if(ap_system.arming_light) {
            digitalWrite(A_LED_PIN, LED_ON);
        }else{
            digitalWrite(A_LED_PIN, LED_OFF);
        }
    }else{
        // armed
        if(!ap_system.arming_light) {
            ap_system.arming_light = true;
            digitalWrite(A_LED_PIN, LED_ON);
        }
    }
}

static void dancing_light()
{
    static uint8_t step;

    if (step++ == 3)
        step = 0;

    switch(step)
    {
    case 0:
        digitalWrite(C_LED_PIN, LED_OFF);
        digitalWrite(A_LED_PIN, LED_ON);
        break;

    case 1:
        digitalWrite(A_LED_PIN, LED_OFF);
        digitalWrite(B_LED_PIN, LED_ON);
        break;

    case 2:
        digitalWrite(B_LED_PIN, LED_OFF);
        digitalWrite(C_LED_PIN, LED_ON);
        break;
    }
}

static void clear_leds()
{
    digitalWrite(A_LED_PIN, LED_OFF);
    digitalWrite(B_LED_PIN, LED_OFF);
    digitalWrite(C_LED_PIN, LED_OFF);
    ap_system.arming_light = false;
    led_mode = NORMAL_LEDS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Blimp LEDS
//  Based on Copter LEDS by Robert Lefebvre
//	Based on the work of U4eake, Bill Sanford, Max Levine, and Oliver
//	g.blimp_leds_mode controls the blimp leds function via bitmath
//	Zeroeth bit turns motor leds on and off:                                00000001
//	First bit turns GPS function on and off:                                00000010
//	Second bit turns Aux function on and off:                               00000100
//	Third bit turns on Beeper (legacy Piezo) function:                      00001000
//	Fourth bit toggles between Fast Flash or Oscillate on Low Battery:      00010000		(0) does Fast Flash, (1) does Oscillate
//	Fifth bit causes motor LEDs to Nav Blink:                               00100000
//	Sixth bit causes GPS LEDs to Nav Blink:                                 01000000
//	This code is written in order to be backwards compatible with the old Motor_LEDS code
//	I hope to include at least some of the Show_LEDS code in the future
//	blimp_leds_GPS_blink controls the blinking of the GPS LEDS
//	blimp_leds_motor_blink controls the blinking of the motor LEDS
//	Piezo Code and beeps once on Startup to verify operation
//	Piezo Enables Tone on reaching low battery or current alert
/////////////////////////////////////////////////////////////////////////////////////////////

#define BLIMP_LEDS_BITMASK_ENABLED         0x01        // bit #0
#define BLIMP_LEDS_BITMASK_GPS             0x02        // bit #1
#define BLIMP_LEDS_BITMASK_AUX             0x04        // bit #2
#define BLIMP_LEDS_BITMASK_BEEPER          0x08        // bit #3
#define BLIMP_LEDS_BITMASK_BATT_OSCILLATE  0x10        // bit #4
#define BLIMP_LEDS_BITMASK_MOTOR_NAV_BLINK 0x20        // bit #5
#define BLIMP_LEDS_BITMASK_GPS_NAV_BLINK   0x40        // bit #6

#if BLIMP_LEDS == ENABLED
static void blimp_leds_init(void)
{
    pinMode(BLIMP_LED_1, OUTPUT);              //Motor LED
    pinMode(BLIMP_LED_2, OUTPUT);              //Motor LED
    pinMode(BLIMP_LED_3, OUTPUT);              //Motor LED
    pinMode(BLIMP_LED_4, OUTPUT);              //Motor LED
    pinMode(BLIMP_LED_5, OUTPUT);              //Motor or Aux LED
    pinMode(BLIMP_LED_6, OUTPUT);              //Motor or Aux LED
    pinMode(BLIMP_LED_7, OUTPUT);              //Motor or GPS LED
    pinMode(BLIMP_LED_8, OUTPUT);              //Motor or GPS LED

    if (!(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER)) {
        piezo_beep();
    }
}

static void update_blimp_leds(void)
{
    if (g.blimp_leds_mode == 0) {
        blimp_leds_reset();                                        //method of reintializing LED state
    }

    // motor leds control
    if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_ENABLED) {
        if (motors.armed()) {
            if (ap.low_battery) {
                if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BATT_OSCILLATE) {
                    blimp_leds_oscillate();                        //if motors are armed, but battery level is low, motor leds fast blink
                } else {
                    blimp_leds_fast_blink();                       //if motors are armed, but battery level is low, motor leds oscillate
                }
            } else {
                if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_MOTOR_NAV_BLINK) {
                    if (blimp_leds_nav_blink > 0) {
                        blimp_leds_slow_blink();                   //if nav command was seen, blink LEDs.
                    } else {
                        blimp_leds_on();
                    }
                } else {
                    blimp_leds_on();                               //if motors are armed, battery level OK, all motor leds ON
                }
            }
        } else {
            blimp_leds_slow_blink();                               //if motors are not armed, blink motor leds
        }
    }

    // GPS led control
    if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS) {

        // GPS LED on if we have a fix or Blink GPS LED if we are receiving data
        // ---------------------------------------------------------------------
        switch (g_gps->status()) {

        case GPS::NO_GPS:
            blimp_leds_GPS_off();                                  //if no valid GPS signal, turn GPS LED off
            break;

        case GPS::NO_FIX:
            blimp_leds_GPS_slow_blink();                           //if GPS has valid reads, but no fix, blink GPS LED slow
            break;

        case GPS::GPS_OK_FIX_2D:
        case GPS::GPS_OK_FIX_3D:
            if(ap.home_is_set) {
                if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS_NAV_BLINK) {
                    if (blimp_leds_nav_blink >0) {
                        blimp_leds_GPS_slow_blink();               //if nav command was seen, blink LEDs.
                    } else {
                        blimp_leds_GPS_on();
                    }
                } else {
                    blimp_leds_GPS_on();							//Turn GPS LEDs on when gps has valid fix AND home is set
                }
            } else {
                blimp_leds_GPS_fast_blink();                       //if GPS has fix, but home is not set, blink GPS LED fast
            }
            break;
        }
    }

    // AUX led control
    if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_AUX) {
        if (ap_system.CH7_flag) {
            blimp_leds_aux_on();                                   //if sub-control of Ch7 is high, turn Aux LED on
        } else {
            blimp_leds_aux_off();                                  //if sub-control of Ch7 is low, turn Aux LED off
        }
    }
}

static void blimp_leds_reset(void) {
    digitalWrite(BLIMP_LED_1, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_2, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_3, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_4, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_5, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_6, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_7, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_8, BLIMP_LED_OFF);
}

static void blimp_leds_on(void) {
    if (!(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_AUX)) {
        digitalWrite(BLIMP_LED_1, BLIMP_LED_ON);
    }
 #if CONFIG_HAL_BOARD == HAL_BOARD_APM2
    if (!(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER)) {
        digitalWrite(BLIMP_LED_2, BLIMP_LED_ON);
    }
 #else
    digitalWrite(BLIMP_LED_2, BLIMP_LED_ON);
 #endif
    if (!(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS) ) {
        digitalWrite(BLIMP_LED_3, BLIMP_LED_ON);
    }
    digitalWrite(BLIMP_LED_4, BLIMP_LED_ON);
    digitalWrite(BLIMP_LED_5, BLIMP_LED_ON);
    digitalWrite(BLIMP_LED_6, BLIMP_LED_ON);
    digitalWrite(BLIMP_LED_7, BLIMP_LED_ON);
    digitalWrite(BLIMP_LED_8, BLIMP_LED_ON);
}

static void blimp_leds_off(void) {
    if (!(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_AUX)) {
        digitalWrite(BLIMP_LED_1, BLIMP_LED_OFF);
    }
 #if CONFIG_HAL_BOARD == HAL_BOARD_APM2
    if (!(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER)) {
        digitalWrite(BLIMP_LED_2, BLIMP_LED_OFF);
    }
 #else
    digitalWrite(BLIMP_LED_2, BLIMP_LED_OFF);
 #endif
    if (!(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS)) {
        digitalWrite(BLIMP_LED_3, BLIMP_LED_OFF);
    }
    digitalWrite(BLIMP_LED_4, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_5, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_6, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_7, BLIMP_LED_OFF);
    digitalWrite(BLIMP_LED_8, BLIMP_LED_OFF);
}

static void blimp_leds_slow_blink(void) {
    blimp_leds_motor_blink++;                                                  // this increments once every 1/10 second because it is in the 10hz loop

    if ( 0 < blimp_leds_motor_blink && blimp_leds_motor_blink < 6 ) {         // when the counter reaches 5 (1/2 sec), then toggle the leds
        blimp_leds_off();

        // if blinking is called by the Nav Blinker decrement the Nav Blink counter
        if ((g.blimp_leds_mode & BLIMP_LEDS_BITMASK_MOTOR_NAV_BLINK) && !(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS_NAV_BLINK) && blimp_leds_nav_blink >0 ) {
            blimp_leds_nav_blink--;
        }
    }else if (5 < blimp_leds_motor_blink && blimp_leds_motor_blink < 11) {
        blimp_leds_on();
    }else{
        blimp_leds_motor_blink = 0;                                            // start blink cycle again
    }
}

static void blimp_leds_fast_blink(void) {    
    blimp_leds_motor_blink++;                                                  // this increments once every 1/10 second because it is in the 10hz loop
    if ( 0 < blimp_leds_motor_blink && blimp_leds_motor_blink < 3 ) {         // when the counter reaches 3 (1/5 sec), then toggle the leds
        blimp_leds_on();
    }else if (2 < blimp_leds_motor_blink && blimp_leds_motor_blink < 5) {
        blimp_leds_off();
    }else{
        blimp_leds_motor_blink = 0;                                            // start blink cycle again
    }
}

static void blimp_leds_oscillate(void) {
    blimp_leds_motor_blink++;                                                  // this increments once every 1/10 second because it is in the 10hz loop
    if ( 0 < blimp_leds_motor_blink && blimp_leds_motor_blink < 3 ) {         // when the counter reaches 3 (1/5 sec), then toggle the leds
        if ( !(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_AUX)) {
            digitalWrite(BLIMP_LED_1, BLIMP_LED_ON);
        }
 #if CONFIG_HAL_BOARD == HAL_BOARD_APM2
        if ( !(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER)) {
            digitalWrite(BLIMP_LED_2, BLIMP_LED_ON);
        }
 #else
        digitalWrite(BLIMP_LED_2, BLIMP_LED_ON);
 #endif
        if ( !(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS)) {
            digitalWrite(BLIMP_LED_3, BLIMP_LED_OFF);
        }
        digitalWrite(BLIMP_LED_4, BLIMP_LED_OFF);
        digitalWrite(BLIMP_LED_5, BLIMP_LED_ON);
        digitalWrite(BLIMP_LED_6, BLIMP_LED_ON);
        digitalWrite(BLIMP_LED_7, BLIMP_LED_OFF);
        digitalWrite(BLIMP_LED_8, BLIMP_LED_OFF);
    }else if (2 < blimp_leds_motor_blink && blimp_leds_motor_blink < 5) {
        if ( !(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_AUX)) {
            digitalWrite(BLIMP_LED_1, BLIMP_LED_OFF);
        }
 #if CONFIG_HAL_BOARD == HAL_BOARD_APM2
        if ( !(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER)) {
            digitalWrite(BLIMP_LED_2, BLIMP_LED_OFF);
        }
 #else
        digitalWrite(BLIMP_LED_2, BLIMP_LED_OFF);
 #endif
        if ( !(g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS) ) {
            digitalWrite(BLIMP_LED_3, BLIMP_LED_ON);
        }
        digitalWrite(BLIMP_LED_4, BLIMP_LED_ON);
        digitalWrite(BLIMP_LED_5, BLIMP_LED_OFF);
        digitalWrite(BLIMP_LED_6, BLIMP_LED_OFF);
        digitalWrite(BLIMP_LED_7, BLIMP_LED_ON);
        digitalWrite(BLIMP_LED_8, BLIMP_LED_ON);
    }else{
        blimp_leds_motor_blink = 0;                                            // start blink cycle again
    }
}

static void blimp_leds_GPS_on(void) {
    digitalWrite(BLIMP_LED_3, BLIMP_LED_ON);
}

static void blimp_leds_GPS_off(void) {
    digitalWrite(BLIMP_LED_3, BLIMP_LED_OFF);
}

static void blimp_leds_GPS_slow_blink(void) {
    blimp_leds_GPS_blink++;                                                    // this increments once every 1/10 second because it is in the 10hz loop
    if ( 0 < blimp_leds_GPS_blink && blimp_leds_GPS_blink < 6 ) {             // when the counter reaches 5 (1/2 sec), then toggle the leds
        blimp_leds_GPS_off();
        if ((g.blimp_leds_mode & BLIMP_LEDS_BITMASK_GPS_NAV_BLINK) && blimp_leds_nav_blink >0 ) {   // if blinking is called by the Nav Blinker...
            blimp_leds_nav_blink--;                                                                                                                    // decrement the Nav Blink counter
        }
    }else if (5 < blimp_leds_GPS_blink && blimp_leds_GPS_blink < 11) {
        blimp_leds_GPS_on();
    }
    else blimp_leds_GPS_blink = 0;                                             // start blink cycle again
}

static void blimp_leds_GPS_fast_blink(void) {
    blimp_leds_GPS_blink++;                                                    // this increments once every 1/10 second because it is in the 10hz loop
    if ( 0 < blimp_leds_GPS_blink && blimp_leds_GPS_blink < 3 ) {             // when the counter reaches 3 (1/5 sec), then toggle the leds
        blimp_leds_GPS_off();
    }else if (2 < blimp_leds_GPS_blink && blimp_leds_GPS_blink < 5) {
        blimp_leds_GPS_on();
    }
    else blimp_leds_GPS_blink = 0;                                             // start blink cycle again
}

static void blimp_leds_aux_off(void){
    digitalWrite(BLIMP_LED_1, BLIMP_LED_OFF);
}

static void blimp_leds_aux_on(void){
    digitalWrite(BLIMP_LED_1, BLIMP_LED_ON);
}

void piezo_on(){
    if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER) {
        digitalWrite(PIEZO_PIN,HIGH);
    }
}

void piezo_off(){
    if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER) {
        digitalWrite(PIEZO_PIN,LOW);
    }
}

void piezo_beep(){                                                              // Note! This command should not be used in time sensitive loops
    if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER) {
        piezo_on();
        delay(100);
        piezo_off();
    }
}

void piezo_beep_twice(){                                                        // Note! This command should not be used in time sensitive loops
    if (g.blimp_leds_mode & BLIMP_LEDS_BITMASK_BEEPER) {
        piezo_beep();
        delay(50);
        piezo_beep();
    }
}

#endif                  //BLIMP_LEDS