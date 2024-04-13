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
    + = D8

  -Vibrator
    -D3 = right
    -D2 = left
*/

#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <TFLI2C.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>
#include "wifi_pass.h"

//LAN 
const char ssid [] = LAN_SSID;
const char pass [] = LAN_PASS;
WiFiClient ardClient;

// Server
const char mqttServer [] = BROKER_IP;
const int mqttPort = 1883;
PubSubClient mqttClient(ardClient);

//Topics in the MQTT
const char topic[] = "LIDAR_DATA"; //could be remove
const char topic1[] = "SERVO_CTRL";
const char topic2[] = "DETECTION";

//TFLuna Lidar
TFLI2C tflI2C;
int16_t tfDist;
int16_t tfAddr = TFL_DEF_ADR;
String lid_data;

//Servo
 Servo servo1;
 int servoPin = 6;

//Vibrator
//change pin if necessary
const int vibmotorL = 2; //Left
const int vibmotorR = 3; //right

//Speaker
const int chipSelect = 8;

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

void wifi_reconnect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Wifi connection lost. Attemting to reconnect...");
    wifi_setup();
  }
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

void mqtt_reconnect() //reconnecting to the server
{
  if (!mqttClient.connected()) 
  {
    Serial.println("MQTT connection lost. Attempting to reconnect...");
    if (mqttClient.connect("arduinoClient")) 
    {
      Serial.println("Reconnected to MQTT broker");
      mqttClient.subscribe(topic1); // Re-subscribe to topics after reconnection
      mqttClient.subscribe(topic2);
      //mqttClient.publish(topic,message);
    } 
    
    else 
    {
      Serial.print("Reconnection failed. Error code = ");
      Serial.println(mqttClient.state());
      delay(5000); // Wait before retrying
      return;
    }
  }
}

void sd_func()
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
  sd_func();

  File myFile13 = SD.open("care.wav");
  File myFile14 = SD.open("chair.wav");
  File myFile15 = SD.open("close.wav");
  File myFile16 = SD.open("door.wav");
  File myFile17 = SD.open("far.wav");
  File myFile18 = SD.open("found.wav");
  File myFile19 = SD.open("front.wav");
  File myFile20 = SD.open("left.wav");
  File myFile21 = SD.open("meters.wav");
  File myFile22 = SD.open("near.wav");
  File myFile23 = SD.open("obs.wav");
  File myFile24 = SD.open("person.wav");
  File myFile25 = SD.open("right.wav");
  File myFile26 = SD.open("stairs.wav");
  File myFile27 = SD.open("table.wav");
  File myFile28 = SD.open("poste.wav");
  //File myFile29 = SD.open("table.wav");
  File myFile30 = SD.open("obstraction.wav");
  File myFile31 = SD.open("ahead.wav");
  File myFile32 = SD.open("warning.wav");
  
  /* if (!myFile)  //code for checkin if the wav file is available
     {
        Serial.println("Error opening testA.wav");
        while (true);
     }
  */

  
  //callback messages
  //Serial.println("Message arrived on topic: " + String(topic));

  //

  if (strcmp(topic, topic2) == 0) 
  {
    String detection = "";
    for (int i = 0; i < length; i++)
    {
      detection +=(char)payload[i];
    }

    Serial.print("Payload: ");
    Serial.print(detection);
    
    if (detection.indexOf("PERSON") != -1)
    {
      Serial.println("Person Detected!");
      AudioZero.play(myFile24);
      myFile24.close();
    }

    else if (detection.indexOf("CHAIR") != -1)
    {
      Serial.println("Chair Detected!");
      AudioZero.play(myFile14);
      myFile14.close();
    }

    else if (detection.indexOf("DOOR") != -1)
    {
      Serial.println("Door Detected!");
      AudioZero.play(myFile16);
      myFile16.close();
    }

    else if (detection.indexOf("STAIRS") != -1)
    {
      Serial.println("Stairs Detected!");
      AudioZero.play(myFile26);
      myFile26.close();
    }

    else if (detection.indexOf("TABLE") != -1)
    {
      Serial.println("Table Detected!");
      AudioZero.play(myFile27);
      myFile27.close();
    }

    else if (detection.indexOf("POSTE") != -1)
    {
      Serial.println("Poste Detected!");
      AudioZero.play(myFile28);
      myFile28.close();
    }
  }

  else if (strcmp(topic, topic1) == 0) 
  {
    // Convert message to integer (assuming message contains servo position)
    int servoPos = atoi((char *)payload);

    // Set servo position based on received message
    Serial.println("Servo position set to: " + String(servoPos)); //could be commented
    servo1.write(servoPos);

    if (servoPos == 90)
    {
      delay(50);
      lidar_func();

      if (tfDist <= 75) //continous vibration of both left and right vibrator motor
      {
        Serial.println("warning collision");
        AudioZero.play(myFile32);
        myFile32.close();
        digitalWrite(vibmotorL, HIGH); 
        delay(10);
        digitalWrite(vibmotorR, HIGH);
        delay(10);
      } 

      else if (tfDist > 75 && tfDist <= 100) //continous vibration of both left and right vibrator motor
      {
        Serial.println("obstraction close front becareful");
        AudioZero.play(myFile30);
        AudioZero.play(myFile15);
        AudioZero.play(myFile19);
        AudioZero.play(myFile13);
        myFile30.close();
        myFile15.close();
        myFile19.close();
        myFile13.close();
      } 
      
      else if (tfDist > 100 && tfDist < 200)
      {
        Serial.println("obstraction near front");
        AudioZero.play(myFile30);
        AudioZero.play(myFile22);
        AudioZero.play(myFile19);
        myFile30.close();
        myFile22.close();
        myFile19.close();
      }

      else if (tfDist >= 200 && tfDist < 300)
      {
        Serial.println("obstraction far front");
        AudioZero.play(myFile30);
        AudioZero.play(myFile17);
        AudioZero.play(myFile19);
        myFile30.close();
        myFile17.close();
        myFile19.close();
      }
    }

    else if (servoPos == 45 || 55 || 65 || 75 || 85) //left
    {
      delay(50);
      lidar_func();
      if (tfDist <= 75) //continous vibration of both left and right vibrator motor
      {
        Serial.println("warning collision");
        AudioZero.play(myFile32);
        myFile32.close();
        digitalWrite(vibmotorL, HIGH); 
        delay(10);
        digitalWrite(vibmotorR, HIGH);
        delay(10);
      } 

      else if (tfDist > 75 && tfDist <= 100) 
      {
        Serial.println("obstraction close left becareful");
        AudioZero.play(myFile30);
        AudioZero.play(myFile15);
        AudioZero.play(myFile20);
        AudioZero.play(myFile13);
        myFile30.close();
        myFile15.close();
        myFile20.close();
        myFile13.close();
      } 
      
      else if (tfDist > 100 && tfDist < 200)
      {
        Serial.println("obstraction near left");
        AudioZero.play(myFile30);
        AudioZero.play(myFile22);
        AudioZero.play(myFile20);
        myFile30.close();
        myFile22.close();
        myFile20.close();
      }

      else if (tfDist >= 200 && tfDist < 300)
      {
        Serial.println("obstraction far left");
        AudioZero.play(myFile30);
        AudioZero.play(myFile17);
        AudioZero.play(myFile20);
        myFile30.close();
        myFile17.close();
        myFile20.close();
      }

    }

    else if (servoPos == 95 || 105 || 115 || 125 || 135) //right
    {
      delay(50);
      lidar_func();
       
       if (tfDist <= 75) //continous vibration of both left and right vibrator motor
      {
        Serial.println("warning collision");
        AudioZero.play(myFile32);
        myFile32.close();
        digitalWrite(vibmotorL, HIGH); 
        delay(10);
        digitalWrite(vibmotorR, HIGH);
        delay(10);
      }

      else if (tfDist > 75 && tfDist <= 100) 
      {
        Serial.println("obstraction close right becareful");
        AudioZero.play(myFile30);
        AudioZero.play(myFile15);
        AudioZero.play(myFile25);
        AudioZero.play(myFile13);
        myFile30.close();
        myFile15.close();
        myFile25.close();
        myFile13.close();
      } 
      
      else if (tfDist > 100 && tfDist < 200)
      {
        Serial.println("obstraction near right");
        AudioZero.play(myFile30);
        AudioZero.play(myFile22);
        AudioZero.play(myFile25);
        myFile30.close();
        myFile22.close();
        myFile25.close();
      }

      else if (tfDist >= 200 && tfDist < 300)
      {
        Serial.println("obstraction far right");
        AudioZero.play(myFile30);
        AudioZero.play(myFile17);
        AudioZero.play(myFile25);
        myFile30.close();
        myFile17.close();
        myFile25.close();
      }
    }
    delay(50);
  }
  
}

void setup() 
{
  Serial.begin(115200);
  AudioZero.begin(16000);
  //while(!Serial);
  Wire.begin();
  wifi_setup();
  broker_setup();
  mqttClient.setCallback(callback);
  mqttClient.subscribe(topic1);
  mqttClient.subscribe(topic2);
  sd_func();
  servo1.attach(servoPin);
  pinMode(vibmotorL, OUTPUT);
  pinMode(vibmotorR, OUTPUT);
}

void loop() 
{
  wifi_reconnect();  
  mqtt_reconnect();
  mqttClient.loop();
}


