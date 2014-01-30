/**
 * @file: commanager.cpp
 * @date: Dec 31, 2013
 */

#include "commanager.h"
#include <msp430.h>
#include "defines.h"

// initialize static members
Queue ComManager::gcsQ;
Queue ComManager::airshipQ;
Queue ComManager::rotorcraftQ;
uint16_t ComManager::txTimers[NUM_COM_IDS] = {0, 0, 0};
bool ComManager::updateTimers = false;
ComId ComManager::gcsFromId = AIRSHIP;
ComId ComManager::airshipFromId = ROTORCRAFT;
ComId ComManager::rotorcraftFromId = GCS;
uint16_t ComManager::gcsTxLen = 0;
uint16_t ComManager::airshipTxLen = 0;
uint16_t ComManager::rotorcraftTxLen = 0;
uint16_t ComManager::gcsTxCount = 0;
uint16_t ComManager::airshipTxCount = 0;
uint16_t ComManager::rotorcraftTxCount = 0;

ComManager* ComManager::GetInstance()
{
    static ComManager manager;
    return &manager;
}

ComManager::ComManager()
{
    // initialize route table
    routeTable[GCS] = GCS_ID;
    routeTable[AIRSHIP] = AIRSHIP_ID;
    routeTable[ROTORCRAFT] = ROTORCRAFT_ID;

    _DINT();  // disable interrupts until we go into the main loop

    InitTimer();
    InitUsci();
}

ComManager::~ComManager()
{
}

void ComManager::Mainloop()
{
    while (true)
    {
        _DINT();  // disable interrupts until we go into low-power mode

        if (gcsQ.IsDataAvailable(ROTORCRAFT_LINK_GCS_HEAD) || airshipQ.IsDataAvailable(ROTORCRAFT_LINK_AIRSHIP_HEAD))
        {
            UCA0IE |= UCTXIE; // enable USCI A0 TX interrupt
        }
        if (airshipQ.IsDataAvailable(GCS_LINK_AIRSHIP_HEAD) || rotorcraftQ.IsDataAvailable(GCS_LINK_ROTORCRAFT_HEAD))
        {
            UCA1IE |= UCTXIE; // enable USCI A1 TX interrupt
        }
        //TODO: enable I2C TX interrupt

        _BIS_SR(LPM0_bits | GIE); // enter low-power mode with interrupt enable

        WDTCTL = WDT_CONFIG; // pet watchdog

        // decrement timers if needed
        if (updateTimers)
        {
            UpdateTimers();
            updateTimers = false;
        }

        // check for Rotorcraft (UART A0) TX interrupt
        if (UCA0IFG & UCTXIFG)
        {
            // check if we have finished sending a message
            if (rotorcraftTxCount == rotorcraftTxLen)
            {
                bool foundMsg = false;
                // check both links to see if there is a message to send
                for (int i = 0; !foundMsg && i < 2; ++i)
                {
                    if (rotorcraftFromId == GCS)
                    {
                        rotorcraftFromId = AIRSHIP;
                        foundMsg = airshipQ.FindNextMessage(ROTORCRAFT_LINK_AIRSHIP_HEAD, routeTable[ROTORCRAFT], rotorcraftTxLen);
                    }
                    else
                    {
                        rotorcraftFromId = GCS;
                        foundMsg = gcsQ.FindNextMessage(ROTORCRAFT_LINK_GCS_HEAD, routeTable[ROTORCRAFT], rotorcraftTxLen);
                    }
                }
                if (foundMsg)
                {
                    rotorcraftTxCount = 0;
                }
            }

            // check if we have a message to send
            if (rotorcraftTxCount < rotorcraftTxLen)
            {
                // find which link we're currently receiving data from
                switch (rotorcraftFromId)
                {
                case GCS:
                    if (gcsQ.IsDataAvailable(ROTORCRAFT_LINK_GCS_HEAD))
                    {
                        UCA0TXBUF = gcsQ.Dequeue(ROTORCRAFT_LINK_GCS_HEAD);
                        txTimers[ROTORCRAFT] = TX_TIMEOUT; // reset timer
                        ++rotorcraftTxCount;
                    }
                    break;
                case AIRSHIP:
                    if (airshipQ.IsDataAvailable(ROTORCRAFT_LINK_AIRSHIP_HEAD))
                    {
                        UCA0TXBUF = airshipQ.Dequeue(ROTORCRAFT_LINK_AIRSHIP_HEAD);
                        txTimers[ROTORCRAFT] = TX_TIMEOUT; // reset timer
                        ++rotorcraftTxCount;
                    }
                    break;
                default:
                    // we should never get here
                    break;
                }
            }
        }

        // check for GCS (UART A1) TX interrupt
        if (UCA1IFG & UCTXIFG)
        {
            // check if we have finished sending a message
            if (gcsTxCount == gcsTxLen)
            {
                bool foundMsg = false;
                // check both links to see if there is a message to send
                for (int i = 0; !foundMsg && i < 2; ++i)
                {
                    if (gcsFromId == ROTORCRAFT)
                    {
                        gcsFromId = AIRSHIP;
                        foundMsg = airshipQ.FindNextMessage(GCS_LINK_AIRSHIP_HEAD, routeTable[GCS], gcsTxLen);
                    }
                    else
                    {
                        gcsFromId = ROTORCRAFT;
                        foundMsg = rotorcraftQ.FindNextMessage(GCS_LINK_ROTORCRAFT_HEAD, routeTable[GCS], gcsTxLen);
                    }
                }
                if (foundMsg)
                {
                    gcsTxCount = 0;
                }
            }

            // check if we have a message to send
            if (gcsTxCount < gcsTxLen)
            {
                // find which link we're currently receiving data from
                switch (gcsFromId)
                {
                case AIRSHIP:
                    if (airshipQ.IsDataAvailable(GCS_LINK_AIRSHIP_HEAD))
                    {
                        UCA1TXBUF = airshipQ.Dequeue(GCS_LINK_AIRSHIP_HEAD);
                        txTimers[GCS] = TX_TIMEOUT; // reset timer
                        ++gcsTxCount;
                    }
                    break;
                case ROTORCRAFT:
                    if (rotorcraftQ.IsDataAvailable(GCS_LINK_ROTORCRAFT_HEAD))
                    {
                        UCA1TXBUF = rotorcraftQ.Dequeue(GCS_LINK_ROTORCRAFT_HEAD);
                        txTimers[GCS] = TX_TIMEOUT; // reset timer
                        ++gcsTxCount;
                    }
                    break;
                default:
                    // we should never get here
                    break;
                }
            }
        }

        //TODO: check for Airship (I2C) TX interrupt

    } // end while (true)
}

#pragma vector=USCI_A0_VECTOR
__interrupt void ComManager::USCI_A0_ISR()
{
    // check for Rotorcraft (UART A0) RX interrupt
    if (UCA0IFG & UCRXIFG)
    {
        rotorcraftQ.Enqueue(UCA0RXBUF); // enqueue data
    }

    // disable TX interrupts
    UCA0IE &= ~UCTXIE;
    UCA1IE &= ~UCTXIE;

    _BIC_SR_IRQ(LPM0_bits); // exit low-power mode
}

#pragma vector=USCI_A1_VECTOR
__interrupt void ComManager::USCI_A1_ISR()
{
    // check for GCS (UART A1) RX interrupt
    if (UCA1IFG & UCRXIFG)
    {
        gcsQ.Enqueue(UCA1RXBUF); // enqueue data
    }

    // disable TX interrupts
    UCA0IE &= ~UCTXIE;
    UCA1IE &= ~UCTXIE;

    _BIC_SR_IRQ(LPM0_bits); // exit low-power mode
}

#pragma vector=USCI_B1_VECTOR
__interrupt void ComManager::USCI_B1_ISR()
{
    // check for Airship (I2C) interrupts
    switch (__even_in_range(UCB1IV, USCI_I2C_UCTXIFG))
    {
    case USCI_I2C_UCRXIFG:
        airshipQ.Enqueue(UCB1RXBUF); // enqueue data
        break;
    case USCI_I2C_UCTXIFG:
        //TODO: dequeue data
        break;
    default:
        break;
    }

    // disable TX interrupts
    UCA0IE &= ~UCTXIE;
    UCA1IE &= ~UCTXIE;

    _BIC_SR_IRQ(LPM0_bits); // exit low-power mode
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ComManager::TIMER0_A0_ISR()
{
    updateTimers = true;

    // disable TX interrupts and exit low-power mode
    UCA0IE &= ~UCTXIE;
    UCA1IE &= ~UCTXIE;
    _BIC_SR_IRQ(LPM0_bits);
}

void ComManager::InitTimer()
{
    // configure Timer A to generate interrupt every 500 ms
    TA0CTL = TASSEL__ACLK | MC__UP | ID__1; // select ACLK / 1 in up mode
    TA0CCR0 = 16383;                        // 2 Hz
    TA0CCTL0 = CCIE;                        // Capture/Compare interrupt enable
}

void ComManager::InitUsci()
{
    // initialize UART A0
    UCA0PSEL |= UCA0TXBIT | UCA0RXBIT; // set UCA0TXD and UCA0RXD to transmit and receive data
    UCA0CTL1 |= UCSWRST;               // hold in reset while configuring
    UCA0CTL0 = 0;
    UCA0CTL1 |= UCSSEL__SMCLK;         // clock source SMCLK - 12 MHz
    UCA0BR0 = 208;                     // 12 MHz / 57,600 bits per second
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_2 | UCBRF_0;      // modulation pattern
    UCA0CTL1 &= ~UCSWRST;              // release reset
    UCA0IE |= UCRXIE;                  // enable USCI A0 RX interrupt

    // initialize UART A1
    UCA1PSEL |= UCA1TXBIT | UCA1RXBIT; // set UCA1TXD and UCA1RXD to transmit and receive data
    UCA1CTL1 |= UCSWRST;               // hold in reset while configuring
    UCA1CTL0 = 0;
    UCA1CTL1 |= UCSSEL__SMCLK;         // clock source SMCLK - 12 MHz
    UCA1BR0 = 208;                     // 12 MHz / 57,600 bits per second
    UCA1BR1 = 0;
    UCA1MCTL = UCBRS_2 | UCBRF_0;      // modulation pattern
    UCA1CTL1 &= ~UCSWRST;              // release reset
    UCA1IE |= UCRXIE;                  // enable USCI A1 RX interrupt

    // initialize I2C
    P4SEL |= BIT2 | BIT1;
    UCB1CTL1 |= UCSWRST;               // hold in reset while configuring
    UCB1CTL0 = UCMODE_3 | UCSYNC;      // 7-bit address (default), single master (default), slave mode (default), I2C mode, sync
    UCB1I2COA = I2C_SLAVE_ADDRESS;     // set own I2C address
    UCB1CTL1 &= ~UCSWRST;              // release reset
    UCB1IE |= UCRXIE;
}
