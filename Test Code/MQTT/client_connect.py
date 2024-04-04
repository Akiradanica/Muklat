import paho.mqtt.client as mqtt 

def on_message(client, userdata, message):
    print("Received message: ", str(message.payload.decode("utf-8")))

client = mqtt.Client("akira23")  # Choose a unique client ID
client.on_message = on_message

client.connect("192.168.0.101", 1883)  # Connect to MQTT broker
client.subscribe("TEMPERATURE")  # Subscribe to a topic
client.loop_forever()  # Keep the client connected

# Don't forget to replace "broker_address" and "topic" with your actual MQTT broker address and topic.
