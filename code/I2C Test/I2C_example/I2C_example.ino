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
const uint8_t GCS_HEADER[GCS_HEADER_LEN] = {0xFE, 0xFF, 0xFE, 0x00, 0x00, 0x01, 0x00, 0xF0};

const uint SETUP_MSG_LEN = 17;
const uint8_t SETUP_MSG_GCS[SETUP_MSG_LEN] = {0xFE, 0xFF, 0xFE, 0x07, 0x00, 0x01, 0x00, 0xF0, 's', 'e', 't', 'u', 'p', '(', ')', 0x00, 0x00};
const uint8_t SETUP_MSG_QUAD[SETUP_MSG_LEN] = {0xFE, 0x02, 0xFE, 0x07, 0x00, 0x01, 0x00, 0xF0, 's', 'e', 't', 'u', 'p', '(', ')', 0x00, 0x00};

const uint ERROR_MSG_LEN = 16;
const uint8_t ERROR_MSG_GCS[ERROR_MSG_LEN] = {0xFE, 0xFF, 0xFE, 0x06, 0x00, 0x01, 0x00, 0xF0, 'E', 'R', 'R', 'O', 'R', '!', 0x00, 0x00};

// global variables
Queue toRouterQ;
Queue fromRouterQ;
uint msgLen = 0;
uint8_t tempBuff[MAX_MESSAGE_LEN];
bool readyRead = false;

// function prototypes
bool readI2c();
void sendDebugMsg(const uint8_t *data, uint dataLen);

void setup()
{
  Wire.begin(); // join I2C bus
  Serial.begin(57600);
  Serial.println("Setup done");

  char msg[] = "setup()";
  sendDebugMsg((uint8_t *)msg, 7);

  pinMode(I2C_INT_PIN, INPUT);
  attachInterrupt(0, readyReadIsr, RISING);
}

void loop()
{
  // check for UART to I2C data
  while (Serial.available() > 0)
  {
    toRouterQ.Enqueue(Serial.read());
  }
  
  if (msgLen == 0 && toRouterQ.Length() > JAGUAR_LEN_OFFSET)
  {
    msgLen = JAGUAR_HEADER_LEN + MAVLINK_HEADER_LEN + MAVLINK_TAIL_LEN + toRouterQ.View(JAGUAR_LEN_OFFSET);
  }

  if (toRouterQ.Length() >= msgLen)
  {
    for (uint i = 0; i < msgLen; ++i)
    {
      tempBuff[i] = toRouterQ.Dequeue();
    }
    
    uint index = 0;
    while (index < msgLen)
    {
      Wire.beginTransmission(I2C_SLAVE_ADDRESS);
      index += Wire.write(&(tempBuff[index]), min(msgLen - index, I2C_BUFF_SIZE));
      Wire.endTransmission();
    }
    msgLen = 0;
  }

  // check for I2C to UART data
  if (readyRead)
  {
    bool ok = readI2c();
    
    if (ok)
    {
      for (uint i = fromRouterQ.Length(); i > 0; --i)
      {
        Serial.write(fromRouterQ.Dequeue());
      }
    }
  }
}

bool readI2c()
{
  readyRead = false;

  uint rxCount = 0;
  bool ok = true;

  // request first 2 bytes (to get the message length)
  rxCount = Wire.requestFrom(I2C_SLAVE_ADDRESS, MIN_LEN_FOR_RX);
  if (rxCount == MIN_LEN_FOR_RX)
  {
    for (uint i = 0; i < MIN_LEN_FOR_RX; ++i)
    {
      fromRouterQ.Enqueue(Wire.read());
    }
  }
  else
  {
    ok = false;
  }

  // request rest of message
  if (ok)
  {
    uint msgLen = fromRouterQ.View(MAVLINK_LEN_OFFSET) - MIN_LEN_FOR_RX;
    rxCount = Wire.requestFrom(I2C_SLAVE_ADDRESS, msgLen);
    if (rxCount == msgLen)
    {
      for (uint i = 0; i < msgLen; ++i)
      {
        fromRouterQ.Enqueue(Wire.read());
      }
    }
    else
    {
      ok = false;
    }
  }
  
  // if something went wrong, clear the queue
  if (!ok)
  {
    fromRouterQ.Clear();
  }
  
  return ok;
}

void sendDebugMsg(const uint8_t *data, uint dataLen)
{
  uint len = JAGUAR_HEADER_LEN + MAVLINK_HEADER_LEN + MAVLINK_TAIL_LEN + dataLen;
  uint8_t buff[len];
  
  // insert message header
  memcpy(buff, GCS_HEADER, GCS_HEADER_LEN);
  buff[JAGUAR_LEN_OFFSET] = dataLen;
  
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

void readyReadIsr()
{
  readyRead = true;
//  Serial.println("!");
}

