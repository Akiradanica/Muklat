/*
  Used pins
  -LIDAR
    SDA/A4/D18
    SCL/A5/D19

  -Servo
    D6

  -SD CARD
    CK - D13
    MISO - D12
    MOSI - D11
    SCK - D10

  -SPEAKER
    + = D9
    - = Transistor

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
const char ssid [] = "For Thesis ONLY";
const char pass [] = "HAHA-05-haha";
WiFiClient ardClient;

// Server
const char mqttServer [] = "192.168.1.19";
const int mqttPort = 1883;
PubSubClient mqttClient(ardClient);

//Topics in the MQTT
const char topic[] = "LIDAR_DATA";
const char topic2[] = "VIB_CTRL"; //pwedeng iremove na siguro toh
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

  //disp connection to network and broker/server
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

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}  

void Mqtt_reconnect() //reconnecting to the server
{
  if (!mqttClient.connected()) 
  {
    Serial.println("MQTT connection lost. Attempting to reconnect...");
    if (mqttClient.connect("arduinoClient")) 
    {
      Serial.println("Reconnected to MQTT broker");
      mqttClient.subscribe(topic); // Re-subscribe to topics after reconnection
      mqttClient.subscribe(topic2);
      mqttClient.subscribe(topic3);
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

    if (tfDist < 50) 
    {
      vibrLEFT_func(); 
    } 
    else
    {
      vibrRIGHT_func();
      
    }
 
  }
  delay(50);
  
}

/*void servo_func() //servo setup
{
  mqttClient.subscribe("SERVO_CTRL");
  
  
  servo1.write(0);
  delay(1000);
  servo1.write(90);
  delay(1000);
  servo1.write(180);
  delay(1000);
  servo1.write(90);
  delay(1000);

}*/

//"SERVO_CTRL";
//"AUD_OUT";

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.println("Message arrived on topic: " + String(topic));

  if (strcmp(topic, "SERVO_CTRL") == 0) 
  {
    // Convert message to integer (assuming message contains servo position)
    int servoPos = atoi((char *)payload);

    // Set servo position based on received message
    
    Serial.println("Servo position set to: " + String(servoPos));
    servo1.write(servoPos);
    //delay(10);
    lidar_func();
  }
  else if (strcmp(topic, topic4) == 0)
  {
    // Handle audio output message
    // Example: parse and process audio output control
    Serial.println("Received audio output control");
    // Add your audio output processing code here
  }
  
}

void speaker_func()
{
  
}
 
void vibrLEFT_func() //need to still improve
{
  digitalWrite(vibmotorL, HIGH);
  /*if ()
  {
    digitalWrite(vibmotorL, HIGH);
  }
  else 
  {
    digitalWrite(vibmotorL, LOW);
  }*/
}


void vibrRIGHT_func() //need to still improve
{
  digitalWrite(vibmotorL, LOW);
}

void setup() 
{
  Serial.begin(115200);
  //while(!Serial);
  Wire.begin();
  wifi_setup();
  broker_setup();
  mqttClient.setCallback(callback);
  mqttClient.subscribe("SERVO_CTRL");
  mqttClient.subscribe("AUD_OUT");
  servo1.attach(servoPin);
  pinMode(vibmotorL, OUTPUT);
  //pinMode(vibmotorR, OUTPUT);
}

void loop() 
{
  //wifi_reconnect();  //stil not added
  Mqtt_reconnect();
  mqttClient.loop();
  
}


