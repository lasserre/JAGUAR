/**
 * @file main.cpp
 */

#include <msp430.h>
#include "defines.h"
#include "commanager.h"

/**
 * Set the VCore to the specified level
 * @param level
 */
void ClockSetVCoreUp(unsigned int level);

/**
 * Initialize clock modules
 * MCLK  = 24 MHz
 * SMCLK = 12 MHz
 * ACLK  = 32,768 Hz
 */
void initClocks();

extern "C"
{

int _system_pre_init()
{
    WDTCTL = WDT_CONFIG; // config watchdog timer

    initClocks();

    return 1;
}

} // extern "C"

int main()
{
    ComManager* managerPtr = ComManager::GetInstance();
    managerPtr->Mainloop();

    return 0;
}

void ClockSetVCoreUp(unsigned int level)
{
    PMMCTL0_H = PMMPW_H; // Open PMM registers for write
    SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level; // Set SVS/SVM high side new level
    SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level; // Set SVM low side to new level
    while ((PMMIFG & SVSMLDLYIFG) == 0); // Wait until SVM is settled
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG); // Clear already set flags
    PMMCTL0_L = PMMCOREV0 * level; // Set VCore to new level
    if (PMMIFG & SVMLIFG)
    {
       while ((PMMIFG & SVMLVLRIFG) == 0); // Wait till new level reached
    }
    SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level; // Set SVS/SVM low side to new level
    PMMCTL0_H = 0x00; // Lock PMM registers for write access
}

void initClocks()
{
    // increase VCore, step by step, to support 24 MHz
    ClockSetVCoreUp(1);
    ClockSetVCoreUp(2);
    ClockSetVCoreUp(3);

    UCSCTL3 |= SELREF__REFOCLK;                                  // set DCO FLL reference to REFO
    UCSCTL4 = SELA__REFOCLK | SELS__DCOCLKDIV | SELM__DCOCLKDIV; // ACLK = REFO, SMCLK = DCOCLKDIV, MCLK = DCOCLKDIV
    UCSCTL5 |= DIVS__2;                                          // set SMCLK divider to 2

    __bis_SR_register(SCG0); // disable the FLL control loop

    UCSCTL0 = 0;             // set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_7;     // select DCO range 24 MHz operation
    UCSCTL2 = FLLD__2 | 731; // set FLL Div = fDCOCLK/2
                             // set DCO multiplier for 24 MHz: (731 + 1) * 32,768 = 24 MHz

    __bic_SR_register(SCG0); // enable the FLL control loop

    __delay_cycles(752000);  // worst case settling time for DCO

    // loop until XT1, XT2, and DCO fault flags are cleared
    do
    {
        UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG); // clear XT2, XT1, and DCO fault flags
        SFRIFG1 &= ~OFIFG;                          // clear fault flags
    } while (SFRIFG1 & OFIFG);
}
