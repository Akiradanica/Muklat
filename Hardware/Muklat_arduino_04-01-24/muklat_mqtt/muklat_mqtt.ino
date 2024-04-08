#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <TFLI2C.h>
#include "wifi_pass.h"

//LAN 
const char ssid [] = LAN_SSID;
const char pass [] = LAN_PASS;
WiFiClient ardClient;

// Server
const char mqttServer [] = BROKER_IP;
const int mqttPort = BROKER_PORT;
PubSubClient mqttClient(ardClient);

//Topics in the MQTT
const char topic[] = "LIDAR_DATA";
const char topic2[] = "VIB_CTRL";
const char topic3[] = "SERVO_CTRL";
const char topic4[] = "AUD_OUT";

//TFLuna
TFLI2C tflI2C;
int16_t tfDist;
int16_t tfAddr = TFL_DEF_ADR;
String lid_data;

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

void lidar_func()
{
  if (tflI2C.getData(tfDist, tfAddr)) 
  {
    String lid_data = String(tfDist) + " cm / " + String(tfDist / 100) + " m";
    Serial.println(lid_data);
    const char* payload = lid_data.c_str();
    mqttClient.publish(topic, payload);
  }
  delay(50);
}

void setup() 
{
  Serial.begin(115200);
  while(!Serial);
  Wire.begin();
  wifi_setup();
  broker_setup();
}

void loop() 
{
  //wifi_reconnect();  //stil not added
  Mqtt_reconnect();
  mqttClient.loop();
  lidar_func();
  

}


