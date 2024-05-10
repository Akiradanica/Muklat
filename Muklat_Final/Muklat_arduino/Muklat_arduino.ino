/*
  Pins used
  -LIDAR
    SDA/A4/D18
    SCL/A5/D19
 
  -Servo
    D6
 
  -SD CARD
    Cs - D10
    MISO - D12
    MOSI - D11
    SCK - D13
 
  -Amplifier/SPEAKER
    + = D0 /d14
 
  -Vibrator
    -D4 = right
    -D3 = left
*/
 
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <TFLI2C.h>
#include <Servo.h>
#include <SD.h>
#include <AudioZero.h>
#include "wifi_pass.h"
 
// LAN
char ssid [] = LAN_SSID;
char pass [] = LAN_PASS;
WiFiClient ardClient;
 
// Server
char mqttServer [] = BROKER_IP;
int mqttPort = 1883;
PubSubClient mqttClient(ardClient);
 
// Topics in the MQTT
char topic[] = "LIDAR_DATA"; // could be removed
char topic1[] = "SERVO_CTRL";
char topic2[] = "DETECTION";
 
// TFLuna Lidar
TFLI2C tflI2C;
int16_t tfDist;
int16_t tfAddr = TFL_DEF_ADR;
char lid_data;
 
// Servo
Servo servo1;
int servoPin = 6;
 
// Vibrator
int vibmotorL = 3; // Left
int vibmotorR = 4; // Right
 
// Speaker
int chipSelect = 10;
 
int detectioncount = 0;
 
void wifi_setup() // setting up wifi connection
{
  Serial.print("Attemting to connect to WPA SSID: ");
  Serial.println(ssid);
 
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
 
  // disp connection to network
  Serial.println("");
  Serial.println("You're connected to the Network");
  Serial.println("");
}
 
void broker_setup() // setting up the server connection
{
  Serial.print("Attenmting to connect to MQTT Broker: ");
  Serial.println(mqttServer);
  Serial.println(" Port: ");
  Serial.println(mqttPort);
 
  mqttClient.setServer(mqttServer, mqttPort);
 
  if (!mqttClient.connect("arduinoClient"))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.state());
 
    while (1);
  }
 
  Serial.println("You're connected to the MQTT broker!"); // disp connection to the server
  Serial.println();
  Serial.println();
  mqttClient.subscribe(topic1);
  mqttClient.subscribe(topic2);
}  
 
void sd_func() // setting up the sd card
{
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect))
  {
    Serial.println("SD card initialization failed!");
    while (true);
  }
  Serial.println("SD card initialized.");
}
 
void lidar_func() // lidar setup
{
  if (tflI2C.getData(tfDist, tfAddr))
  {
    String lid_data = String(tfDist) + " cm / " + String(tfDist / 100) + " m";
    Serial.println(lid_data);
    const char* payload = lid_data.c_str();
    mqttClient.publish(topic, payload);
  }
}
 
void callback(char* topic, byte* payload, unsigned int length) 
{
  // Check if the received message is for servo control
  if (strcmp(topic, topic1) == 0) // topic1: "SERVO_CTRL"
  { 
    // Convert the payload to an integer representing the servo position
    int servoPos = atoi((char *)payload);
    // Call the function to set the servo position
    setServoPosition(servoPos);
  }

  if (strcmp(topic, topic2) == 0) // topic2: "DETECTION"
  { 
    String detection = "";
    for (int i = 0; i < length; i++) 
    {
      detection += (char)payload[i];
    }
    detection[length] = '\0'; // Append null terminator

    handleDetection(detection);
  }
}
 
void handleDetection(String detection)
{
  Serial.println("Detection received: " + detection);
 
  if (detection.equals("person")) 
  {
    Serial.println("Person Detected!");
    playAudio(2);    
    detectioncount++;
  }
 
  else if (detection.equals("chair")) 
  {
    Serial.println("Chair Detected!");
    playAudio(0);
    detectioncount++;
  }
 
  else if (detection.equals("door"))
  {
    Serial.println("Door Detected!");
    playAudio(1);
    detectioncount++;
  }
 
  else if (detection.equals("stairs"))
  {
    Serial.println("Stairs Detected!");
    playAudio(4);
    detectioncount++;
  }
 
  else if (detection.equals("table"))
  {
    Serial.println("Table Detected!");
    playAudio(5);
    detectioncount++;
  }
 
  else if (detection.equals("post"))
  {
    Serial.println("Post Detected!");
    playAudio(3);
    detectioncount++;
  }
 
   // Check if detectioncount is equal to 10
  if (detectioncount >= 10)
  {
    Serial.println("Reset");
    NVIC_SystemReset();
    detectioncount = 0; // Reset detectioncount back to 0
  }
}
 
void setServoPosition(int servoPos)
{
  Serial.print("Setting servo position: ");
  Serial.println(servoPos);
  servo1.write(servoPos);
  
  //FRONT
  // Check if the servo position is in the front range
  if (servoPos >= 80 && servoPos < 100) // approximately 90 degrees
  {
    servoPos = 90;
    delay(50);
    lidar_func();
 
    // Check Lidar distance for potential collisions
    if (tfDist <= 75) // Continuous vibration of both left and right vibrator motor
    {
      Serial.println("Warning: Collision in front!");
      playAudio(6);
 
      digitalWrite(vibmotorL, HIGH);
      digitalWrite(vibmotorR, HIGH);
      delay(100);
      digitalWrite(vibmotorL, LOW);
      digitalWrite(vibmotorR, LOW);
      delay(100);
    }
    else if (tfDist > 75 && tfDist <= 100) // Close obstruction in front
    {
      Serial.println("Close obstruction in front, be careful!");
      playAudio(7);
    }
    else if (tfDist > 100 && tfDist < 200) // Obstruction near front
    {
      Serial.println("Obstruction near front");
      playAudio(8);
    }
    else if (tfDist >= 200 && tfDist <= 300) // Obstruction far front
    {
      Serial.println("Obstruction far in front");
      playAudio(9);
    }
  }

  // Check if the servo position is on the left side
  else if (servoPos >= 45 && servoPos < 80) // LEFT
  {
    servoPos = 45;
    delay(50);
    lidar_func();
   
    // Check Lidar distance for potential collisions
    if (tfDist <= 75) // Continuous vibration of both left and right vibrator motor
    {
      Serial.println("Warning: Collision on the left side!");
      playAudio(10);
 
      digitalWrite(vibmotorL, HIGH);
      digitalWrite(vibmotorR, HIGH);
      delay(100);
      digitalWrite(vibmotorL, LOW);
      digitalWrite(vibmotorR, LOW);
      delay(100);
    }

    else if (tfDist > 75 && tfDist <= 100) // Close obstruction on the left side
    {
      Serial.println("Close obstruction on the left side, be careful!");
      playAudio(11);
    }

    else if (tfDist > 100 && tfDist < 200) // Obstruction near left
    {
      Serial.println("Obstruction near the left side");
      playAudio(8);
    }

    else if (tfDist >= 200 && tfDist <= 300) // Obstruction far left
    {
      Serial.println("Obstruction far on the left side");
      playAudio(13);
    }
  }
  
  // Check if the servo position is on the right side
  else if (servoPos >= 100 && servoPos < 135) // RIGHT
  {
    servoPos = 135;
    delay(50);
    lidar_func();
       
    // Check Lidar distance for potential collisions
    if (tfDist <= 75) // Continuous vibration of both left and right vibrator motor
    {
      Serial.println("Warning: Collision on the right side!");
      playAudio(14);
 
      digitalWrite(vibmotorL, HIGH);
      digitalWrite(vibmotorR, HIGH);
      delay(100);
      digitalWrite(vibmotorL, LOW);
      digitalWrite(vibmotorR, LOW);
      delay(100);
    }
    else if (tfDist > 75 && tfDist <= 100) // Close obstruction on the right side
    {
      Serial.println("Close obstruction on the right side, be careful!");
      playAudio(15);
    }
    else if (tfDist > 100 && tfDist < 200) // Obstruction near right
    {
      Serial.println("Obstruction near the right side");
      playAudio(16);
    }
    else if (tfDist >= 200 && tfDist <= 300) // Obstruction far right
    {
      Serial.println("Obstruction far on the right side");
      playAudio(17);
    }
  }
  delay(50); // Ensure some delay between servo movements
}

 
void playAudio(int i) //for playing audio files from sd card
{
  String aud[] =
  {
    "chair.wav",     //0
    "door.wav",      //1
    "person.wav",    //2
    "post.wav",      //3
    "stairs.wav",    //4
    "table.wav",     //5
    "f1.wav",        //6
    "f2.wav",        //7
    "f3.wav",        //8
    "f4.wav",        //9
    "l1.wav",        //10
    "l2.wav",        //11
    "l3.wav",        //12
    "l4.wav",        //13
    "r1.wav",        //14
    "r2.wav",        //15
    "r3.wav",        //16
    "r4.wav"        //17
  };
 
  if (i >= 0 && i < 18 && aud[i])
  {
    File myFile = SD.open(aud[i]);
    AudioZero.begin(16000);
    AudioZero.play(myFile);
    AudioZero.end();
    myFile.close();
   
  }
}
 
void setup()
{
  Serial.begin(115200);
  // while(!Serial);
  wifi_setup();
  broker_setup();
  mqttClient.setCallback(callback);
  sd_func();
  Wire.begin();
  servo1.attach(servoPin);
  pinMode(vibmotorL, OUTPUT);
  pinMode(vibmotorR, OUTPUT);
}
 
void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    wifi_setup();
  }
 
  // Reconnect MQTT if disconnected
  if (!mqttClient.connected())
  {
    broker_setup();  // Attempt to reconnect MQTT
  }
 
  // Maintain MQTT connection
  mqttClient.loop();
}