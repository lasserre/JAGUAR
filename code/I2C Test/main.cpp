#include <msp430.h>
#include "defines.h"
#include "msp430g2553.h"

void initUsci();
void initTimerA();

char message;


int main(void)
{
    unsigned int i;
    unsigned int j;

    WDTCTL = WDTPW | WDTHOLD;  // stop watchdog timer

    initUsci();
    initTimerA();

    //P1DIR ^= BIT0;
    //P1OUT &= ~BIT0;

    _EINT();
    
    char first = 'h';
    
    

    _BIS_SR(LPM0_bits | GIE); // enter low-power mode with interrupt enable

    return 0;
}

void initUsci()
{
//    // initialize UART A0
//    P1SEL |= BIT2 | BIT1;
//    P1SEL2 |= BIT2 | BIT1;
//    UCA0CTL1 |= UCSWRST;          // hold in reset while configuring
//    UCA0CTL0 = 0;
//    UCA0CTL1 |= UCSSEL_2;         // clock source SMCLK - 1,048,576 Hz
//    UCA0BR0 = 109;                // 1,048,576 / 9600 bits per second
//    UCA0BR1 = 0;
//    UCA0MCTL = UCBRS_2 | UCBRF_0; // modulation pattern
//    UCA0CTL1 &= ~UCSWRST;         // release reset
//    UC0IE |= UCA0RXIE;            // interrupt enable

    // initialize I2C
    IE2 |= UCB0TXIE | UCB0RXIE;
    P1SEL |= BIT7 | BIT6;
    P1SEL2 |= BIT7 | BIT6;
    UCB0CTL1 |= UCSWRST;               // hold in reset while configuring
    UCB0CTL0 = UCMODE_3 | UCSYNC;      // 7-bit address (default), single master (default), slave mode (default), I2C mode, sync
    UCB0I2COA = I2C_SLAVE_ADDRESS;     // set own I2C address
    UCB0CTL1 &= ~UCSWRST;              // release reset
    UC0IE |= UCB0TXIE | UCB0RXIE;
    UCB0I2CIE |= UCSTTIE;
    
    P2DIR |= BIT3;                      //Set P2.3 as an output port.
    P2SEL |= ~BIT3;
    P2SEL2 |= ~BIT3;
    P2OUT |= BIT3;
}

void initTimerA()
{
    TA0CTL = TASSEL_2 | MC_1 | ID_3; // select SMCLK / 8 and UP mode
    TA0CCR0 = 65535; // 500 ms interval at 1,048,576 Hz
    TA0CCTL0 = CCIE; // Capture/Compare interrupt enable
}

//#pragma vector=USCIAB0TX_VECTOR
//__interrupt void USCI0TX_ISR()
//{
//    if (UC0IFG & UCA0TXIFG)
//    {
//        UCA0TXBUF = 'A';
//    }
//    if (UC0IFG & UCA0TXIFG)
//    {
//        UCB0TXBUF = 'I';
//    }
//}
//
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0RX_ISR()
{
    if (UC0IFG & UCA0RXIFG)
    {
        while (!(UC0IFG & UCA0TXIFG));
        message = UCA0RXBUF;
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR()
{
    static unsigned int count = 0;

    P2OUT &= ~BIT3;

    if (++count == 4)
    {
        while (!(UC0IFG & UCA0TXIFG));
        UCA0TXBUF = 'X';
        count = 0;
    }
    
    P2OUT |= BIT3;

}
