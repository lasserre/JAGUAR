// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#if CLI_ENABLED == ENABLED

// These are function definitions so the Menu can be constructed before the functions
// are defined below. Order matters to the compiler.
#if HIL_MODE != HIL_MODE_ATTITUDE && HIL_MODE != HIL_MODE_SENSORS
static int8_t   test_baro(uint8_t argc,                 const Menu::arg *argv);
#endif
static int8_t   test_battery(uint8_t argc,              const Menu::arg *argv);
static int8_t   test_compass(uint8_t argc,              const Menu::arg *argv);
static int8_t   test_eedump(uint8_t argc,               const Menu::arg *argv);
static int8_t   test_gps(uint8_t argc,                  const Menu::arg *argv);
static int8_t   test_ins(uint8_t argc,                  const Menu::arg *argv);
static int8_t   test_logging(uint8_t argc,              const Menu::arg *argv);
static int8_t   test_motors(uint8_t argc,               const Menu::arg *argv);
#if OPTFLOW == ENABLED
static int8_t   test_optflow(uint8_t argc,              const Menu::arg *argv);
#endif
static int8_t   test_radio_pwm(uint8_t argc,            const Menu::arg *argv);
static int8_t   test_radio(uint8_t argc,                const Menu::arg *argv);
static int8_t   test_relay(uint8_t argc,                const Menu::arg *argv);
#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
static int8_t   test_shell(uint8_t argc,                const Menu::arg *argv);
#endif
#if HIL_MODE != HIL_MODE_ATTITUDE && HIL_MODE != HIL_MODE_SENSORS && CONFIG_SONAR == ENABLED
static int8_t   test_sonar(uint8_t argc,                const Menu::arg *argv);
#endif
static int8_t   test_tuning(uint8_t argc,               const Menu::arg *argv);

// This is the help function
// PSTR is an AVR macro to read strings from flash memory
// printf_P is a version of printf that reads from flash memory
/*static int8_t	help_test(uint8_t argc,             const Menu::arg *argv)
 *  {
 *       cliSerial->printf_P(PSTR("\n"
 *                                                "Commands:\n"
 *                                                "  radio\n"
 *                                                "  servos\n"
 *                                                "  g_gps\n"
 *                                                "  imu\n"
 *                                                "  battery\n"
 *                                                "\n"));
 *  }*/

// Creates a constant array of structs representing menu options
// and stores them in Flash memory, not RAM.
// User enters the string in the console to call the functions on the right.
// See class Menu in AP_Coommon for implementation details
const struct Menu::command test_menu_commands[] PROGMEM = {
#if HIL_MODE != HIL_MODE_ATTITUDE && HIL_MODE != HIL_MODE_SENSORS
    {"baro",                test_baro},
#endif
    {"battery",             test_battery},
    {"compass",             test_compass},
    {"eedump",              test_eedump},
    {"gps",                 test_gps},
    {"ins",                 test_ins},
    {"logging",             test_logging},
    {"motors",              test_motors},
#if OPTFLOW == ENABLED
    {"optflow",             test_optflow},
#endif
    {"pwm",                 test_radio_pwm},
    {"radio",               test_radio},
    {"relay",               test_relay},
#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
    {"shell", 				test_shell},
#endif
#if HIL_MODE != HIL_MODE_ATTITUDE && HIL_MODE != HIL_MODE_SENSORS && CONFIG_SONAR == ENABLED
    {"sonar",               test_sonar},
#endif
    {"tune",                test_tuning}
};

// A Macro to create the Menu
MENU(test_menu, "test", test_menu_commands);

static int8_t
test_mode(uint8_t argc, const Menu::arg *argv)
{
    test_menu.run();
    return 0;
}

#if HIL_MODE != HIL_MODE_ATTITUDE && HIL_MODE != HIL_MODE_SENSORS
static int8_t
test_baro(uint8_t argc, const Menu::arg *argv)
{
    int32_t alt;
    print_hit_enter();
    init_barometer();

    while(1) {
        delay(100);
        alt = read_barometer();

        if (!barometer.healthy) {
            cliSerial->println_P(PSTR("not healthy"));
        } else {
            cliSerial->printf_P(PSTR("Alt: %0.2fm, Raw: %f Temperature: %.1f\n"),
                            alt / 100.0,
                            barometer.get_pressure(), 0.1*barometer.get_temperature());
        }
        if(cliSerial->available() > 0) {
            return (0);
        }
    }
    return 0;
}
#endif

static int8_t
test_battery(uint8_t argc, const Menu::arg *argv)
{
    // check if radio is calibration
    pre_arm_rc_checks();
    if(!ap.pre_arm_rc_check) {
        cliSerial->print_P(PSTR("radio not calibrated, exiting"));
        return(0);
    }

    cliSerial->printf_P(PSTR("\nCareful! Motors will spin! Press Enter to start.\n"));
    while (cliSerial->read() != -1); /* flush */
    while(!cliSerial->available()) { /* wait for input */
        delay(100);
    }
    while (cliSerial->read() != -1); /* flush */
    print_hit_enter();

    // allow motors to spin
    output_min();
    motors.armed(true);

    while(1) {
        delay(100);
        read_radio();
        read_battery();
        if (g.battery_monitoring == BATT_MONITOR_VOLTAGE_ONLY) {
            cliSerial->printf_P(PSTR("V: %4.4f\n"),
                            battery_voltage1,
                            current_amps1,
                            current_total1);
        } else {
            cliSerial->printf_P(PSTR("V: %4.4f, A: %4.4f, Ah: %4.4f\n"),
                            battery_voltage1,
                            current_amps1,
                            current_total1);
        }
        motors.throttle_pass_through();

        if(cliSerial->available() > 0) {
            motors.armed(false);
            return (0);
        }
    }
    motors.armed(false);
    return (0);
}

static int8_t
test_compass(uint8_t argc, const Menu::arg *argv)
{
    uint8_t delta_ms_fast_loop;

    if (!g.compass_enabled) {
        cliSerial->printf_P(PSTR("Compass: "));
        print_enabled(false);
        return (0);
    }

    if (!compass.init()) {
        cliSerial->println_P(PSTR("Compass initialisation failed!"));
        return 0;
    }

    ahrs.init();
    ahrs.set_fly_forward(true);
    ahrs.set_compass(&compass);
    report_compass();

    // we need the AHRS initialised for this test
    ins.init(AP_InertialSensor::COLD_START, 
             ins_sample_rate,
             flash_leds);
    ahrs.reset();
    int16_t counter = 0;
    float heading = 0;

    print_hit_enter();

    while(1) {
        delay(20);
        if (millis() - fast_loopTimer > 19) {
            delta_ms_fast_loop      = millis() - fast_loopTimer;
            G_Dt                    = (float)delta_ms_fast_loop / 1000.f;                       // used by DCM integrator
            fast_loopTimer          = millis();

            // INS
            // ---
            ahrs.update();

            medium_loopCounter++;
            if(medium_loopCounter == 5) {
                if (compass.read()) {
                    // Calculate heading
                    const Matrix3f &m = ahrs.get_dcm_matrix();
                    heading = compass.calculate_heading(m);
                    compass.null_offsets();
                }
                medium_loopCounter = 0;
            }

            counter++;
            if (counter>20) {
                if (compass.healthy) {
                    Vector3f maggy = compass.get_offsets();
                    cliSerial->printf_P(PSTR("Heading: %ld, XYZ: %d, %d, %d,\tXYZoff: %6.2f, %6.2f, %6.2f\n"),
                                    (wrap_360_cd(ToDeg(heading) * 100)) /100,
                                    (int)compass.mag_x,
                                    (int)compass.mag_y,
                                    (int)compass.mag_z,
                                    maggy.x,
                                    maggy.y,
                                    maggy.z);
                } else {
                    cliSerial->println_P(PSTR("compass not healthy"));
                }
                counter=0;
            }
        }
        if (cliSerial->available() > 0) {
            break;
        }
    }

    // save offsets. This allows you to get sane offset values using
    // the CLI before you go flying.
    cliSerial->println_P(PSTR("saving offsets"));
    compass.save_offsets();
    return (0);
}

static int8_t
test_eedump(uint8_t argc, const Menu::arg *argv)
{

    // hexdump the EEPROM
    for (uint16_t i = 0; i < EEPROM_MAX_ADDR; i += 16) {
        cliSerial->printf_P(PSTR("%04x:"), i);
        for (uint16_t j = 0; j < 16; j++)  {
            int b = hal.storage->read_byte(i+j);
            cliSerial->printf_P(PSTR(" %02x"), b);
        }
        cliSerial->println();
    }
    return(0);
}

static int8_t
test_gps(uint8_t argc, const Menu::arg *argv)
{
    print_hit_enter();
    delay(1000);

    while(1) {
        delay(100);

        // Blink GPS LED if we don't have a fix
        // ------------------------------------
        update_GPS_light();

        g_gps->update();

        if (g_gps->new_data) {
            cliSerial->printf_P(PSTR("Lat: "));
            print_latlon(cliSerial, g_gps->latitude);
            cliSerial->printf_P(PSTR(", Lon "));
            print_latlon(cliSerial, g_gps->longitude);
            cliSerial->printf_P(PSTR(", Alt: %ldm, #sats: %d\n"),
                            g_gps->altitude/100,
                            g_gps->num_sats);
            g_gps->new_data = false;
        }else{
            cliSerial->print_P(PSTR("."));
        }
        if(cliSerial->available() > 0) {
            return (0);
        }
    }
    return 0;
}

static int8_t
test_ins(uint8_t argc, const Menu::arg *argv)
{
    Vector3f gyro, accel;
    print_hit_enter();
    cliSerial->printf_P(PSTR("INS\n"));
    delay(1000);

    ahrs.init();
    ins.init(AP_InertialSensor::COLD_START, 
             ins_sample_rate,
             flash_leds);

    delay(50);

    while(1) {
        ins.update();
        gyro = ins.get_gyro();
        accel = ins.get_accel();

        float test = accel.length() / GRAVITY_MSS;

        cliSerial->printf_P(PSTR("a %7.4f %7.4f %7.4f g %7.4f %7.4f %7.4f t %74f | %7.4f\n"),
            accel.x, accel.y, accel.z,
            gyro.x, gyro.y, gyro.z,
            test);

        delay(40);
        if(cliSerial->available() > 0) {
            return (0);
        }
    }
}

/*
 *  test the dataflash is working
 */
static int8_t
test_logging(uint8_t argc, const Menu::arg *argv)
{
    cliSerial->println_P(PSTR("Dataflash logging test is disabled for ArduBlimp"));
#if 0 //TODO: enable if needed - JBW
    cliSerial->println_P(PSTR("Testing dataflash logging"));
    DataFlash.ShowDeviceInfo(cliSerial);
#endif // #if 0
    return 0;
}

static int8_t
test_motors(uint8_t argc, const Menu::arg *argv)
{
    cliSerial->printf_P(PSTR(
                        "Connect battery for this test.\n"
                        "Motors will spin in channel order (1,2,3,4,5,6,7,8).\n"
                        "Remember to disconnect battery after this test.\n"
                        "Any key to exit.\n"));

    // ensure all values have been sent to motors
    motors.set_update_rate(g.rc_speed);
    motors.set_frame_orientation(g.frame_orientation);
    motors.set_min_throttle(g.throttle_min);
    motors.set_mid_throttle(g.throttle_mid);
    motors.set_max_throttle(g.throttle_max);

    // enable motors
    init_rc_out();

    while(1) {
        delay(20);
        read_radio();
        motors.output_test();
        if(cliSerial->available() > 0) {
            g.esc_calibrate.set_and_save(0);
            return(0);
        }
    }
}

#if OPTFLOW == ENABLED
static int8_t
test_optflow(uint8_t argc, const Menu::arg *argv)
{
#if OPTFLOW == ENABLED
    #error "OPTFLOW = ENABLED"
#endif
    if(g.optflow_enabled) {
        cliSerial->printf_P(PSTR("man id: %d\t"),optflow.read_register(ADNS3080_PRODUCT_ID));
        print_hit_enter();

        while(1) {
            delay(200);
            optflow.update(millis());
            Log_Write_Optflow();
            cliSerial->printf_P(PSTR("x/dx: %d/%d\t y/dy %d/%d\t squal:%d\n"),
                            optflow.x,
                            optflow.dx,
                            optflow.y,
                            optflow.dy,
                            optflow.surface_quality);

            if(cliSerial->available() > 0) {
                return (0);
            }
        }
    } else {
        cliSerial->printf_P(PSTR("OptFlow: "));
        print_enabled(false);
    }
    return (0);
}
#endif // OPTFLOW == ENABLED

static int8_t
test_radio_pwm(uint8_t argc, const Menu::arg *argv)
{
    print_hit_enter();
    delay(1000);

    while(1) {
        delay(20);

        // Filters radio input - adjust filters in the radio.pde file
        // ----------------------------------------------------------
        read_radio();

        // servo Yaw
        //APM_RC.OutputCh(CH_7, g.rc_4.radio_out);

        cliSerial->printf_P(PSTR("IN  1: %4d  2: %4d  3: %4d  4: %4d  5: %4d  6: %4d  7: %4d  8: %4d\n"),
                        g.rc_1.radio_in,
                        g.rc_2.radio_in,
                        g.rc_3.radio_in,
                        g.rc_4.radio_in,
                        g.rc_5.radio_in,
                        g.rc_6.radio_in,
                        g.rc_7.radio_in,
                        g.rc_8.radio_in);

        if(cliSerial->available() > 0) {
            return (0);
        }
    }
}

static int8_t
test_radio(uint8_t argc, const Menu::arg *argv)
{
    print_hit_enter();
    delay(1000);

    while(1) {
        delay(20);
        read_radio();


        cliSerial->printf_P(PSTR("IN  1: %4d  2: %4d  3: %4d  4: %4d  5: %4d  6: %4d  7: %4d  8: %4d\n"),
                        g.rc_1.control_in,
                        g.rc_2.control_in,
                        g.rc_3.control_in,
                        g.rc_4.control_in,
                        g.rc_5.control_in,
                        g.rc_6.control_in,
                        g.rc_7.control_in,
                        g.rc_8.control_in);

        //cliSerial->printf_P(PSTR("OUT 1: %d\t2: %d\t3: %d\t4: %d\n"), (g.rc_1.servo_out / 100), (g.rc_2.servo_out / 100), g.rc_3.servo_out, (g.rc_4.servo_out / 100));

        /*cliSerial->printf_P(PSTR(	"min: %d"
         *                                               "\t in: %d"
         *                                               "\t pwm_in: %d"
         *                                               "\t sout: %d"
         *                                               "\t pwm_out %d\n"),
         *                                               g.rc_3.radio_min,
         *                                               g.rc_3.control_in,
         *                                               g.rc_3.radio_in,
         *                                               g.rc_3.servo_out,
         *                                               g.rc_3.pwm_out
         *                                               );
         */
        if(cliSerial->available() > 0) {
            return (0);
        }
    }
}

static int8_t test_relay(uint8_t argc, const Menu::arg *argv)
{
    print_hit_enter();
    delay(1000);

    while(1) {
        cliSerial->printf_P(PSTR("Relay on\n"));
        relay.on();
        delay(3000);
        if(cliSerial->available() > 0) {
            return (0);
        }

        cliSerial->printf_P(PSTR("Relay off\n"));
        relay.off();
        delay(3000);
        if(cliSerial->available() > 0) {
            return (0);
        }
    }
}

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
/*
 *  run a debug shell
 */
static int8_t
test_shell(uint8_t argc, const Menu::arg *argv)
{
    hal.util->run_debug_shell(cliSerial);
    return 0;
}
#endif

#if HIL_MODE != HIL_MODE_ATTITUDE && HIL_MODE != HIL_MODE_SENSORS && CONFIG_SONAR == ENABLED
/*
 *  test the sonar
 */
static int8_t
test_sonar(uint8_t argc, const Menu::arg *argv)
{
    if(g.sonar_enabled == false) {
        cliSerial->printf_P(PSTR("Sonar disabled\n"));
        return (0);
    }

    // make sure sonar is initialised
    init_sonar();

    print_hit_enter();
    while(1) {
        delay(100);

        cliSerial->printf_P(PSTR("Sonar: %d cm\n"), sonar->read());

        if(cliSerial->available() > 0) {
            return (0);
        }
    }
    return (0);
}
#endif

static int8_t
test_tuning(uint8_t argc, const Menu::arg *argv)
{
    cliSerial->printf_P(PSTR("Tuning disabled for ArduBlimp\n"));
    return 0;
#if 0 //TODO: enable if needed - JBW
    print_hit_enter();

    while(1) {
        delay(200);
        read_radio();
        tuning();
        cliSerial->printf_P(PSTR("tune: %1.3f\n"), tuning_value);

        if(cliSerial->available() > 0) {
            return (0);
        }
    }
#endif // #if 0
}

static void print_hit_enter()
{
    cliSerial->printf_P(PSTR("Hit Enter to exit.\n\n"));
}

#endif // CLI_ENABLED
