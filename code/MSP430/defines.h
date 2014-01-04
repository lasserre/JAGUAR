/*
 * @file defines.h
 * @date Dec 18, 2013
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define MAVLINK_STX          0xFE

#define MAVLINK_HEADER_SIZE  7
#define MAVLINK_TAIL_SIZE    2

// offsets of bytes in MAVLink header
#define MAVLINK_LEN_OFFSET   1
#define MAVLINK_DEST_OFFSET  6

// destination for sending to all receivers in network
#define MAVLINK_DEST_ALL     0

// communication
#define UCA0PSEL   P3SEL
#define UCA0TXBIT  BIT3
#define UCA0RXBIT  BIT4
#define UCA1PSEL   P4SEL
#define UCA1TXBIT  BIT4
#define UCA1RXBIT  BIT5

#endif /* DEFINES_H_ */
