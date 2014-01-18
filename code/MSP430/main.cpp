/**
 * @file main.cpp
 */

#include <msp430.h>
#include "defines.h"
#include "commanager.h"

extern "C"
{

int _system_pre_init()
{
    WDTCTL = WDT_CONFIG; // config watchdog timer

    return 1;
}

} // extern "C"

int main()
{
    ComManager* managerPtr = ComManager::GetInstance();
    managerPtr->Mainloop();

    return 0;
}
