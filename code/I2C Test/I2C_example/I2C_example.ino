// I2C_example
// configure I2C
// Arduino is a master

#include "Wire.h"
#include "defines.h"
#include "utility/twi.h"

typedef unsigned int uint;

unsigned int MESSAGE_LEN = 13;
byte MESSAGE_TO_GCS[] = {0xFE, 0xFF, 0xFE, 0x03, 0x00, 0x01, 0x00, 0x4A, 'X', 'X', 'X', 0x00, 0x00};
byte MESSAGE_TO_QUAD[] = {0xFE, 0x02, 0xFE, 0x03, 0x00, 0x01, 0x00, 0x4A, 'X', 'X', 'X', 0x00, 0x00};

const uint BUFF_LEN = 512;
byte buff[BUFF_LEN];
uint head = 0;
uint tail = 0;

void enqueue(byte b);
byte dequeue();

void setup()
{
    Wire.begin(); // join I2C bus
//    Wire.onRequest(handlerFunction);
    Serial.begin(57600);
    Serial.println("Setup done");

//    attachInterrupt(0, requestDataISR, FALLING);
}

void loop()
{
  int msgLen = -1;
  
  while (Serial.available() > 0)
  {
    enqueue(Serial.read());
  }

  Serial.println("Sending...");
  Wire.beginTransmission(I2C_SLAVE_ADDRESS);  
//  Wire.write("This is a test. ");
  Wire.write(MESSAGE_TO_GCS, MESSAGE_LEN);
  Wire.endTransmission();

  delay(1500);
}

void enqueue(byte b)
{
  buff[tail] = b;
  if (tail == BUFF_LEN - 1)
  {
    tail = 0;
  }
  else
  {
    ++tail;
  }
}

byte dequeue()
{
  byte b = buff[head];
  if (head == BUFF_LEN - 1)
  {
    head = 0;
  }
  else
  {
    ++head;
  }
  return b;
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




