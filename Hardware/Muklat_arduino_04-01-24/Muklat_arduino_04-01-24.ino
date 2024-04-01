//all are working well and just need to connect to the internet
//speaker not yet connected
#include <Wire.h>
#include <TFLI2C.h>

TFLI2C tflI2C;

int16_t tfDist; // distance in centimeters
int16_t tfAddr = TFL_DEF_ADR; // Use this default I2C address

//int servoMotor = ;
// int speakerPin = 6;

//left 
const int buzzer1Pin = 4;
const int vibmotor1Pin = 3;

//right
//int buzzer2Pin = 7;
//int vibmotor2Pin = 8;

void setup() 
{
  Serial.begin(115200); // Initalize serial port
  Wire.begin();         // Initalize Wire library
  pinMode(buzzer1Pin, OUTPUT);
  //pinMode(buzzer2Pin, OUTPUT);
  pinMode(vibmotor1Pin, OUTPUT);
  //pinMode(vibmotor2Pin, OUTPUT);
  //pinMode(speakerPin, OUTPUT);
  //pinMode(servoMotor, OUTPUT);
}

void loop() 
{
  if (tflI2C.getData(tfDist, tfAddr)) 
  {
    Serial.println(String(tfDist) + " cm / " + String(tfDist / 100) + " m");
    digitalWrite(vibmotor1Pin, HIGH);
    // Check if the distance is less than a threshold (adjust as needed)
    if (tfDist < 50) 
    {
      // Activate buzzer 1 with a frequency of 400
      tone(buzzer1Pin, 400);
      // Activate vibration motor
      digitalWrite(vibmotor1Pin, HIGH);
    } 
    else
    {
      // Stop the buzzer and turn off vibration motor if the distance is greater than the threshold
      noTone(buzzer1Pin);
      digitalWrite(vibmotor1Pin, LOW);
    }
  }
  delay(50);
}
