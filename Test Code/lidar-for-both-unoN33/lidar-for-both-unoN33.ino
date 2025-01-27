// this code is working for both ARDUINO UNO and ARDUINO NANO 33 IOT
//code from 
//https://www.diyengineers.com/2022/06/02/lidar-how-to-use-with-arduino/

#include <Wire.h>
#include <TFLI2C.h>

TFLI2C tflI2C;

int16_t tfDist; // distance in centimeters
int16_t tfAddr = TFL_DEF_ADR; // Use this default I2C address

String lid_data;
void setup() 
{
  Serial.begin(115200); // Initalize serial port
  Wire.begin();         // Initalize Wire library
}

void loop() 
{
  
  if (tflI2C.getData(tfDist, tfAddr)) 
  {
    String lid_data = String(tfDist) + " cm" + String(tfDist / 100) + " m";
    Serial.println(lid_data);
  }
  delay(50);
}
