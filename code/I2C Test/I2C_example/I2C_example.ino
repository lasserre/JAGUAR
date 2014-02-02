// I2C_example
// configure I2C
// Arduino is a master

#include "Wire.h"
#include "defines.h"
#include "utility/twi.h"
#include "queue.h"
#include <stdio.h>

// constants
const uint GCS_HEADER_LEN = JAGUAR_HEADER_LEN + MAVLINK_HEADER_LEN;
const uint8_t GCS_HEADER[GCS_HEADER_LEN] = {0xFE, 0xFF, 0xFE, 0x00, 0x00, 0x01, 0x00, 0xAA};

const uint SETUP_MSG_LEN = 17;
const uint8_t SETUP_MSG_GCS[SETUP_MSG_LEN] = {0xFE, 0xFF, 0xFE, 0x07, 0x00, 0x01, 0x00, 0xAA, 's', 'e', 't', 'u', 'p', '(', ')', 0x00, 0x00};
const uint8_t SETUP_MSG_QUAD[SETUP_MSG_LEN] = {0xFE, 0x02, 0xFE, 0x07, 0x00, 0x01, 0x00, 0xAA, 's', 'e', 't', 'u', 'p', '(', ')', 0x00, 0x00};

const uint ERROR_MSG_LEN = 16;
const uint8_t ERROR_MSG_GCS[ERROR_MSG_LEN] = {0xFE, 0xFF, 0xFE, 0x06, 0x00, 0x01, 0x00, 0xAA, 'E', 'R', 'R', 'O', 'R', '!', 0x00, 0x00};

// global variables
Queue toRouterQ;
uint msgLen = 0;
uint8_t tempBuff[MAX_MESSAGE_LEN];

// function prototypes
void sendDebugMsg(const uint8_t *data, uint dataLen);

void setup()
{
  Wire.begin(); // join I2C bus
//  Wire.onRequest(handlerFunction);
  Serial.begin(57600);
  Serial.println("Setup done");

  char msg[] = "setup()";
  sendDebugMsg((uint8_t *)msg, 7);

//  attachInterrupt(0, requestDataISR, FALLING);
}

void loop()
{
  while (Serial.available() > 0)
  {
    toRouterQ.Enqueue(Serial.read());
  }
  
  if (msgLen == 0 && toRouterQ.Length() > LEN_OFFSET)
  {
    msgLen = JAGUAR_HEADER_LEN + MAVLINK_HEADER_LEN + MAVLINK_TAIL_LEN + toRouterQ.View(LEN_OFFSET);
  }

  if (toRouterQ.Length() >= msgLen)
  {
    for (uint i = 0; i < msgLen; ++i)
    {
      tempBuff[i] = toRouterQ.Dequeue();
    }
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(tempBuff, msgLen);
    Wire.endTransmission();
    msgLen = 0;
  }
}

void sendDebugMsg(const uint8_t *data, uint dataLen)
{
  uint len = JAGUAR_HEADER_LEN + MAVLINK_HEADER_LEN + MAVLINK_TAIL_LEN + dataLen;
  uint8_t buff[len];
  
  // insert message header
  memcpy(buff, GCS_HEADER, GCS_HEADER_LEN);
  buff[LEN_OFFSET] = dataLen;
  
  // insert message payload
  uint index = GCS_HEADER_LEN;
  for (uint i = 0; i < dataLen; ++i)
  {
    buff[index++] = data[i];
  }
  
  // insert message tail
  buff[index++] = 0x00;
  buff[index++] = 0x00;

  Wire.beginTransmission(I2C_SLAVE_ADDRESS);
  Wire.write(buff, len);
  Wire.endTransmission();
}

//void handlerFunction()
//{
//  Serial.print("Received Request! ");
//  Serial.println(x);
//}

//void requestDataISR()
//{
//  Serial.println("Received interrupt!");
//  Wire.beginTransmission(I2C_SLAVE_ADDRESS);
//  Wire.write("Hello MSP430 :)");
//  Wire.endTransmission();
//}




