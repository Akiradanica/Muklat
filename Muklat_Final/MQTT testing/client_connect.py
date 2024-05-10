import paho.mqtt.client as mqtt 

def on_message(client, userdata, message):
    topic = message.topic
    payload = str(message.payload.decode("utf-8"))
    print(f"Received message on topic '{topic}': {payload}")

        
client = mqtt.Client("akira23")  # Choose a unique client ID
client.on_message = on_message

client.connect("192.168.0.108", 1883)  # Connect to MQTT broker
#hotspot ko
#192.168.89.217
#LAN ni mel
#192.168.0.106
client.subscribe("SERVO_CTRL")  # Subscribe to a topic
client.subscribe("DETECTION") 
client.loop_forever()  # Keep the client connected

# Don't forget to replace "broker_address" and "topic" with your actual MQTT broker address and topic.
