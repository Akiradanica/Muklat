import paho.mqtt.client as mqtt 
from random import randrange, uniform
import time

mqttBroker = "192.168.0.101"
#mqttBroker = "localhost"
mqttPort = 1883
client = mqtt.Client("Temperature_Inside")
client.connect(mqttBroker,mqttPort)

while True:
    randNum = uniform(20.0, 21.0)
    client.publish("TEMPERATURE", str(randNum))
    print("Just Published " + str(randNum) + " to Topic TEMPERATURE")
    time.sleep(1)
