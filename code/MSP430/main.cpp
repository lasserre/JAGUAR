/**
 * @file main.cpp
 */

#include <msp430.h>
#include "commanager.h"

int main()
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    ComManager manager;
    manager.Mainloop();

    return 0;
}

