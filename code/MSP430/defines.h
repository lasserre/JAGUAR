/**
 * @file defines.h
 * @date Dec 18, 2013
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdint.h>

// watchdog timer config
#define WDT_CONFIG  (WDTPW | WDTSSEL__ACLK | WDTCNTCL | WDTIS__32K) // timeout is 1 sec

#define MAVLINK_STX          0xFE // the first byte of a MAVLink message

#define MAVLINK_HEADER_SIZE  6
#define MAVLINK_TAIL_SIZE    2

#define MIN_LEN_FOR_ROUTING  4  // the minimum length of the message needed before we can route it

#define JAGUAR_HEADER_LEN    2  // the length of the destination header prepended to the MAVLink message

// offsets of bytes from beginning of JAGUAR destination header
#define JAGUAR_LEN_OFFSET    3
#define JAGUAR_DEST_OFFSET   1

// destination for sending to all receivers in network
#define JAGUAR_DEST_ALL     0

// Tx message timeout (timeout will be TX_TIMEOUT * 1.5 seconds)
#define TX_TIMEOUT          3

// communication
#define UCA0PSEL   P3SEL
#define UCA0TXBIT  BIT3
#define UCA0RXBIT  BIT4
#define UCA1PSEL   P4SEL
#define UCA1TXBIT  BIT4
#define UCA1RXBIT  BIT5

 // ---- Communication ids ---- //
 const uint8_t AIRSHIP_ID = 1;
 const uint8_t ROTORCRAFT_ID = 2;
 const uint8_t GCS_ID = 255;

#endif /* DEFINES_H_ */
