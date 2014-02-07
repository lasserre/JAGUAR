/*
 * defines.h
 *
 *  Created on: Jan 6, 2014
 *      Author: Jonathan
 */

#ifndef DEFINES_H_
#define DEFINES_H_

typedef unsigned int uint;

#define I2C_MASTER_ADDRESS  0x08
#define I2C_SLAVE_ADDRESS   0x09

#define I2C_BUFF_SIZE       32

#define I2C_INT_PIN          2

const uint JAGUAR_HEADER_LEN = 2;
const uint MAVLINK_HEADER_LEN = 6;
const uint MAVLINK_TAIL_LEN = 2;
const uint MAX_MESSAGE_LEN = 265;
const uint JAGUAR_LEN_OFFSET = 3;
const uint MAVLINK_LEN_OFFSET = 1;
const uint MIN_LEN_FOR_RX = 2;     ///< minimum message length needed to receive the message

#endif /* DEFINES_H_ */




