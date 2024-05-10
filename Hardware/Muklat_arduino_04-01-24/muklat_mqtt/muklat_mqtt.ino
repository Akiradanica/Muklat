/*
  Used pins
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

//LAN 
char ssid [] = LAN_SSID;
char pass [] = LAN_PASS;
WiFiClient ardClient;

// Server
char mqttServer [] = BROKER_IP;
int mqttPort = 1883;
PubSubClient mqttClient(ardClient);

//Topics in the MQTT
char topic[] = "LIDAR_DATA"; //could be remove
char topic1[] = "SERVO_CTRL";
char topic2[] = "DETECTION";

//TFLuna Lidar
TFLI2C tflI2C;
int16_t tfDist;
int16_t tfAddr = TFL_DEF_ADR;
char lid_data;

//Servo
 Servo servo1;
 int servoPin = 6;

//Vibrator
//change pin if necessary
int vibmotorL = 3; //Left
int vibmotorR = 4; //right

//Speaker
int chipSelect = 10;

int detectioncount = 0;

void wifi_setup() //setting up wifi connection
{
  Serial.print("Attemting to connect to WPA SSID: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) 
  {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  //disp connection to network
  Serial.println("");
  Serial.println("You're connected to the Network");
  Serial.println("");
}

void broker_setup() //setting up the server connection
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

  Serial.println("You're connected to the MQTT broker!"); //disp connection to the server
  Serial.println();
  Serial.println();
  mqttClient.subscribe(topic1);
  mqttClient.subscribe(topic2);
  
}  

void sd_func() //setting up the sd card
{
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("SD card initialization failed!");
    while (true);
  }
  Serial.println("SD card initialized.");

}


void lidar_func() //lidar setup
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
  

  //checking what topic arrived
  if (strcmp(topic, topic2) == 0) //detection
  {
    // DETECTION topic
    String detection = "";
    for (int i = 0; i < length; i++)
    {
      detection +=(char)payload[i];
    }
    handleDetection(detection);
  }

  else if (strcmp(topic, topic1) == 0) // topic: "SERVO_CTRL"
  {
    // SERVO_CTRL topic
    int servoPos = atoi((char *)payload);
    setServoPosition(servoPos);
  }

  
}

void handleDetection(String detection)
{
  Serial.println("Detection received: " + detection);

  if (detection.equals("PERSON")) //If person detected
  {
    Serial.println("Person Detected!");
    playAudio(2);     
    detectioncount++; 
  }

  else if (detection.equals("CHAIR"))
  {
    Serial.println("Chair Detected!");
    playAudio(0);
    detectioncount++; 

  }

  else if (detection.equals("DOOR"))
  {
    Serial.println("Door Detected!");
    playAudio(1);
    detectioncount++; 

  }

  else if (detection.equals("STAIRS"))
  {
    Serial.println("Stairs Detected!");
    playAudio(4);
    detectioncount++;
  }

  else if (detection.equals("TABLE"))
  {
    Serial.println("Table Detected!");
    playAudio(5);
    detectioncount++;
  }

  else if (detection.equals("POST"))
  {
    Serial.println("Post Detected!");
    playAudio(3);
    detectioncount++;
  }

   // Check if detectioncount is equal to 10
  if (detectioncount >= 10)
  {
    Serial.println("Reset");
    detectioncount = 0; // Reset detectioncount back to 0
    NVIC_SystemReset();
  }
}

void setServoPosition(int servoPos) 
{
  Serial.println("Setting servo position: " + String(servoPos));
  servo1.write(servoPos);

  if (servoPos >= 75 && servoPos < 105) //position 90 deg
  {
    delay(50);
    lidar_func();

    if (tfDist <= 75) //continous vibration of both left and right vibrator motor
    {
      Serial.println("warning collision front");
      playAudio(6);

      digitalWrite(vibmotorL, HIGH); 
      delay(100);
      digitalWrite(vibmotorL, LOW);
      delay(100);
      digitalWrite(vibmotorR, HIGH);
      delay(100);
      digitalWrite(vibmotorR, LOW);
      delay(100);
    } 

    else if (tfDist > 75 && tfDist <= 100) //continous vibration of both left and right vibrator motor
    {
      Serial.println("obstraction close front becareful");
      playAudio(7);
      
    } 
      
    else if (tfDist > 100 && tfDist < 200)
    {
      Serial.println("obstraction near front");
      playAudio(8);
    }

    else if (tfDist >= 200 && tfDist <= 300)
    {
      Serial.println("obstraction far front");
      playAudio(9);
    }
  }

  else if (servoPos >= 105 && servoPos < 135) //left
  {
    delay(50);
    lidar_func();
      
    if (tfDist <= 75) //continous vibration of both left and right vibrator motor
    {
      Serial.println("warning collision left");
      playAudio(10);

      digitalWrite(vibmotorR, LOW);
      delay(100);
      digitalWrite(vibmotorL, HIGH); 
      delay(100);
      digitalWrite(vibmotorL, LOW);
      delay(100);
    } 

    else if (tfDist > 75 && tfDist <= 100) //continous vibration of both left and right vibrator motor
    {
      Serial.println("obstraction close left becareful");
      playAudio(11);

    } 
      
    else if (tfDist > 100 && tfDist < 200)
    {
      Serial.println("obstraction near front");
      playAudio(12);
    }

    else if (tfDist >= 200 && tfDist <= 300)
    {
      Serial.println("obstraction far left");
      playAudio(13);
    }

  }

  else if (servoPos >= 45 && servoPos < 75) //right
  {
    delay(50);
    lidar_func();
       
    if (tfDist <= 75) //continous vibration of both left and right vibrator motor
    {
      Serial.println("warning collision right");
      playAudio(14);

      digitalWrite(vibmotorL, LOW);
      delay(100);
      digitalWrite(vibmotorR, HIGH);
      delay(100);
      digitalWrite(vibmotorR, LOW);
      delay(100);
    } 

    else if (tfDist > 75 && tfDist <= 100) //continous vibration of both left and right vibrator motor
    {
      Serial.println("obstraction close right becareful");
      playAudio(15);
    } 
      
    else if (tfDist > 100 && tfDist < 200)
    {
      Serial.println("obstraction near right");
      playAudio(16);
    }

    else if (tfDist >= 200 && tfDist <= 300)
    {
      Serial.println("obstraction far right");
      playAudio(17);
    }
  }
  delay(50);  
  
}

void playAudio(int i) 
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
  //while(!Serial);
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