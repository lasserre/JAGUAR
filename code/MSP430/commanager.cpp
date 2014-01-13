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

ComManager::ComManager()
{
    gcsFromId = AIRSHIP;
    airshipFromId = ROTORCRAFT;
    rotorcraftFromId = GCS;
    gcsTxLen = 0;
    airshipTxLen = 0;
    rotorcraftTxLen = 0;
    gcsTxCount = 0;
    airshipTxCount = 0;
    rotorcraftTxCount = 0;

    // initialize route table
    routeTable[GCS] = GCS_ID;
    routeTable[AIRSHIP] = AIRSHIP_ID;
    routeTable[ROTORCRAFT] = ROTORCRAFT_ID;

    _DINT();  // disable interrupts until we go into the main loop

    InitTimers();
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
                uint8_t data;
                // find which link we're currently receiving data from
                switch (rotorcraftFromId)
                {
                case GCS:
                    if (gcsQ.Dequeue(ROTORCRAFT_LINK_GCS_HEAD, data))
                    {
                        UCA0TXBUF = data;
                        ++rotorcraftTxCount;
                    }
                    break;
                case AIRSHIP:
                    if (airshipQ.Dequeue(ROTORCRAFT_LINK_AIRSHIP_HEAD, data))
                    {
                        UCA0TXBUF = data;
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
                    if (gcsFromId == GCS)
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
                uint8_t data;
                // find which link we're currently receiving data from
                switch (gcsFromId)
                {
                case AIRSHIP:
                    if (airshipQ.Dequeue(GCS_LINK_AIRSHIP_HEAD, data))
                    {
                        UCA1TXBUF = data;
                        ++gcsTxCount;
                    }
                    break;
                case ROTORCRAFT:
                    if (rotorcraftQ.Dequeue(GCS_LINK_ROTORCRAFT_HEAD, data))
                    {
                        UCA1TXBUF = data;
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
        //TODO: reset timer
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
        //TODO: reset timer
    }

    // disable TX interrupts
    UCA0IE &= ~UCTXIE;
    UCA1IE &= ~UCTXIE;

    _BIC_SR_IRQ(LPM0_bits); // exit low-power mode
}

//TODO: check for Airship (I2C) RX and TX interrupts

void ComManager::InitTimers()
{
    TA0CTL = TASSEL_2 | MC__STOP | ID__8; // select SMCLK / 8
    TA0CCR0 = 65535; // 500 ms interval
    TA0CCTL0 = CCIE; // Capture/Compare interrupt enable
}

void ComManager::InitUsci()
{
    // initialize UART A0
    UCA0PSEL |= UCA0TXBIT | UCA0RXBIT; // set UCA0TXD and UCA0RXD to transmit and receive data
    UCA0CTL1 |= UCSWRST;       // hold in reset while configuring
    UCA0CTL0 = 0;
    UCA0CTL1 |= UCSSEL__SMCLK; // clock source SMCLK - 1,048,576 Hz
    UCA0BR0 = 18;              // 1,048,576 / 57,600 bits per second
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1 | UCBRF_0; // modulation pattern
    UCA0CTL1 &= ~UCSWRST;         // release reset
    UCA0IE |= UCRXIE;             // enable USCI A0 RX interrupt

    // initialize UART A1
    UCA1PSEL |= UCA1TXBIT | UCA1RXBIT; // set UCA1TXD and UCA1RXD to transmit and receive data
    UCA1CTL1 |= UCSWRST;       // hold in reset while configuring
    UCA1CTL0 = 0;
    UCA1CTL1 |= UCSSEL__SMCLK; // clock source SMCLK - 1,048,576 Hz
    UCA1BR0 = 18;              // 1,048,576 / 57,600 bits per second
    UCA1BR1 = 0;
    UCA1MCTL = UCBRS_1 | UCBRF_0; // modulation pattern
    UCA1CTL1 &= ~UCSWRST;         // release reset
    UCA1IE |= UCRXIE;             // enable USCI A1 RX interrupt

    // TODO: initialize I2C
}
