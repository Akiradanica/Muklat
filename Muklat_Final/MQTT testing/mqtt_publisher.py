import paho.mqtt.client as mqtt 
import random
import time

# Define MQTT broker details
mqttBroker = "192.168.0.103"
mqttPort = 1883
client = mqtt.Client("akira_pc")

# Define MQTT topics
topic_detection = "DETECTION"
topic_servo = "SERVO_CTRL"
topic_lidar = "LIDAR_DATA"

# Define the callback function for when a message is received
def on_message(client, userdata, message):
    topic = message.topic
    payload = str(message.payload.decode("utf-8"))
    print(f"Received message on topic '{topic}': {payload}")

# Assign the on_message callback function to the client
#client.on_message = on_message

# Connect to the MQTT broker
client.connect(mqttBroker, mqttPort)

# Start the MQTT client loop
client.loop_start()

# Subscribe to the LIDAR_DATA topic
client.subscribe(topic_lidar)

# Main loop for publishing messages and handling subscriptions
while True:
    # Publish random detection data
    client.on_message = on_message
    
    # Wait for a short duration before publishing the next message
    time.sleep(1)
    
    detections = ["PERSON", "CHAIR", "DOOR", "STAIRS", "TABLE", "POST"]
    random_det = random.choice(detections)
    client.publish(topic_detection, random_det)
    print(f"Just published {random_det} to topic {topic_detection}")
    time.sleep(1)

    # Publish random servo position
    servo_pos = random.randint(45, 135)
    client.publish(topic_servo, str(servo_pos))
    print(f"Just published {servo_pos} to topic {topic_servo}")
    time.sleep(1)

    


# Keep the program running
# client.loop_forever()  # This line would be used if you want to keep the script running indefinitely
