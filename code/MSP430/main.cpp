/**
 * @file main.cpp
 */

#include <msp430.h>
#include "commanager.h"

extern "C" int _system_pre_init()
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    return 1;
}

int main()
{
    ComManager manager;
    manager.Mainloop();

    return 0;
}

