import cv2
import time
import math
from ultralytics import YOLO
import paho.mqtt.client as mqtt

# Constants
KNOWN_DISTANCE = 250  # Assumed distance in centimeters (e.g., 1 meter)
KNOWN_WIDTH = 50  # Assumed width of the reference object's bounding box in centimeters

# MQTT Broker settings
MQTT_HOST = "192.168.0.100"  # Change this to your MQTT broker's address
MQTT_PORT = 1883  # Change this to your MQTT broker's port
DETECTION_TOPIC = "DETECTION"
SERVO_CTRL_TOPIC = "SERVO_CTRL"

# Function to publish MQTT messages
def publish_message(topic, message, client):
    client.publish(topic, message)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
    else:
        print(f"Failed to connect, return code {rc}")

def estimate_distance(known_width, focal_length, per_width):
    # Calculate the distance to the object
    return (known_width * focal_length) / per_width

def angles(angle):
    if 45 <= angle < 85:
        return "left"
    elif 85 <= angle < 95:
        return "front"
    elif 95 <= angle < 135:
        return "right"
    elif 135 <= angle <= 180:
        return "back"
    else:
        return "object not detected"


def webcam():
    # MQTT client initialization
    client = mqtt.Client("akira")
    client.on_connect = on_connect
    client.connect(MQTT_HOST, MQTT_PORT, 60)
    client.loop_start()

    cap = cv2.VideoCapture('http://192.168.0.102:81/stream')  # IP address where the ESP32 is connected
    cap.set(3, 640)
    cap.set(4, 480)

    # Load YOLO model
    model = YOLO("yolo-Weights/CHAIR-PERSON-TABLE/test9/best_9.pt")

    # Object classes
    classNames = [ "chair","person", "table"]
    
    while True:
        success, img = cap.read()
        
        results = model(img, stream=True)

        for r in results:
            boxes = r.boxes

            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

                cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 255), 1)

                # Calculate the width of the bounding box
                per_width = x2 - x1

                # Estimate distance to the object
                distance = estimate_distance(KNOWN_WIDTH, KNOWN_DISTANCE, per_width)

                # Display distance
                cv2.putText(img, f'Distance: {distance:.2f} cm', (x1, y1 + 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                            (0, 255, 0), 1)

                # Determine if the object is on the left or right side of the frame
                obj_center_x = (x1 + x2) // 2

                confidence = math.ceil((box.conf[0] * 100)) / 100
                

                # Calculate angle relative to the center line (90 degrees)
                angle_offset = math.degrees(math.atan((obj_center_x - img.shape[1] // 2) / (img.shape[1] // 2)))

                # Calculate the angle from 1 to 180 degrees, left to right
                angle = 90 + 90 * (angle_offset / (img.shape[1] // 2))

                # Determine position based on angle
                side = angles(angle)

                # Print angle and position to terminal
                cls = int(box.cls[0])
                print("Class name -->", classNames[cls])
                print("confidence", confidence)
                print(f"Angle: {int(angle)}")
                print("Position:", side)

                # Publish detection to MQTT broker
                detection_message = f'{classNames[cls]}'
                publish_message(DETECTION_TOPIC, detection_message, client)

                # Publish position to MQTT broker
                servo_ctrl_message = f'{int(angle)}'
                publish_message(SERVO_CTRL_TOPIC, str(servo_ctrl_message) + " Degrees", client,)

                # Display class name, side, and angle
                cv2.putText(img, f'{classNames[cls]} {confidence} {side}', (x1, y1 + 40),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)

        cv2.imshow('Webcam', img)
        if cv2.waitKey(1) == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    webcam()
