// I2C_example
// configure I2C
// Arduino is a master

#include "Wire.h"
#include "defines.h"
#include "utility/twi.h"


void setup()
{
    Wire.begin(); // join I2C bus
    Wire.onRequest(handlerFunction);
    Serial.begin(9600);
    Serial.print("Setup done");
    Serial.println();

    attachInterrupt(0, requestDataISR, FALLING);
}

byte x = 0;

void loop()
{
//  Serial.print("Waiting ");
//  Serial.println(x);

  Wire.beginTransmission(I2C_SLAVE_ADDRESS);
  Wire.write("T");
  Wire.endTransmission();

  x++;
  if(x>=10) x = 0;
  delay(1000);
}

void handlerFunction()
{
	Serial.print("Received Request! ");
  Serial.println(x);
}

void requestDataISR()
{
//  Serial.println("Received interrupt!");
//  Wire.beginTransmission(I2C_SLAVE_ADDRESS);
//  Wire.write("Hello MSP430 :)");
//  Wire.endTransmission();
}