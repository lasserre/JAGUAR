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

const uint JAGUAR_HEADER_LEN = 2;
const uint MAVLINK_HEADER_LEN = 6;
const uint MAVLINK_TAIL_LEN = 2;
const uint MAX_MESSAGE_LEN = 265;
const uint LEN_OFFSET = 3;

#endif /* DEFINES_H_ */




