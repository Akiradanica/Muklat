#include <Arduino.h>
#include <Wire.h>
#include <TFLI2C.h>

TFLI2C tflI2C;

int16_t  tfDist;    // distance in centimeters
int16_t  tfAddr = TFL_DEF_ADR;  // Use this default I2C address
int buzzer1pin = 4;
int vibmotor1pin = 5;

int buzzer2pin = 7;
int vibmotor2pin = 6;

void setup() 
{
  Serial.begin(115200);  // Initialize serial port
  Wire.begin();           // Initialize Wire library
  pinMode(buzzer1pin, OUTPUT);
  pinMode(vibmotor1pin, OUTPUT);
}

// Add this function to handle the command and send the distance data
void handleCommand(String command) 
{
  if (command == "READ_DISTANCE") 
  {
    if (tflI2C.getData(tfDist, tfAddr)) 
    {
      Serial.println(String(tfDist));
      // buzzer and vibration motor
      if (tfDist < 100) 
      {
        // Activate buzzer 1 with a frequency of 400
        tone(buzzer1pin, 400);
        // Activate vibration motor
        digitalWrite(vibmotor1pin, HIGH);
      } 
      else 
      {
        // Stop the buzzer and turn off vibration motor if the distance is greater than the threshold
        noTone(buzzer1pin);
        digitalWrite(vibmotor1pin, LOW);
      }
    } 
    else 
    {
      Serial.println("Error reading distance");
    }
  }
}



void loop() 
{
  if (Serial.available() > 0) 
  {
    String command = Serial.readStringUntil('\n');
    handleCommand(command);
  }

    // Rest of your existing loop code

    delay(50);
}
