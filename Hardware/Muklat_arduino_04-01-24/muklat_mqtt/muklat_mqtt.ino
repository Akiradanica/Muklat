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
    + = D9

  -Vibrator
    -D3 = right
    -D2 = left
*/

#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <TFLI2C.h>
#include <Servo.h>
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
const char topic[] = "LIDAR_DATA";
const char topic3[] = "SERVO_CTRL";
const char topic4[] = "AUD_OUT";

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
  mqttClient.subscribe(topic3);
  mqttClient.subscribe(topic4);
  
}  

void mqtt_reconnect() //reconnecting to the server
{
  if (!mqttClient.connected()) 
  {
    Serial.println("MQTT connection lost. Attempting to reconnect...");
    if (mqttClient.connect("arduinoClient")) 
    {
      Serial.println("Reconnected to MQTT broker");
      mqttClient.subscribe(topic3); // Re-subscribe to topics after reconnection
      mqttClient.subscribe(topic4);
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

//"SERVO_CTRL";
//"AUD_OUT";

void callback(char* topic, byte* payload, unsigned int length) 
{
  //callback messages
  Serial.println("Message arrived on topic: " + String(topic));
  if (strcmp(topic, topic3) == 0) 
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
      if (tfDist < 75) //continous vibration of both left and right vibrator motor
      {
        digitalWrite(vibmotorL, HIGH); 
        delay(10);
        digitalWrite(vibmotorR, HIGH);
        delay(10);
      } 
    }

    else if (servoPos == 45 || 55 || 65 || 75 || 85)
    {
      delay(50);
      lidar_func();
      if (tfDist < 75) //continous vibration of both left and right vibrator motor
      {
        digitalWrite(vibmotorL, HIGH); 
        digitalWrite(vibmotorR, LOW);
        delay(10);
      }
    }

    else if (servoPos == 95 || 105 || 115 || 125 || 135)
    {
      delay(50);
      lidar_func();
      if (tfDist < 75) //continous vibration of both left and right vibrator motor
      {
        digitalWrite(vibmotorL, LOW); 
        digitalWrite(vibmotorR, HIGH);
        delay(10);
      }
    }
    delay(50);
  }

  else if (strcmp(topic, topic4) == 0)
  {
    // Handle audio output message
    // Example: parse and process audio output control
    Serial.println("Received audio output control"); //could be commented
    //int audioOUT = ;
    //speaker_func();
  }
  
}

void setup() 
{
  Serial.begin(115200);
  //while(!Serial);
  Wire.begin();
  wifi_setup();
  broker_setup();
  mqttClient.setCallback(callback);
  mqttClient.subscribe(topic3);
  mqttClient.subscribe(topic4);
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


