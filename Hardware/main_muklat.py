from subprocess import call
from subprocess import Popen
from ultralytics import YOLO
import cv2
import math
from multiprocessing import Process
import paho.mqtt.client as mqtt 

def on_message(client, userdata, message):
    print("Received message: ", str(message.payload.decode("utf-8")))

    client = mqtt.Client("akira23")  # Choose a unique client ID
    client.on_message = on_message

    client.connect("192.168.0.106", 1883)  # Connect to MQTT broker
    client.subscribe("LIDAR_DATA")  # Subscribe to a topic
    client.loop_forever()  # Keep the client connected

# Don't forget to replace "broker_address" and "topic" with your actual MQTT broker address and topic.

def lidar_buzzer():
    call(["python", "lidar_buzzer.py"])

def webcam():
    cap = cv2.VideoCapture(0)
    #cap = cv2.VideoCapture('http://192.168.214.227:81/stream')
    cap.set(3, 640)
    cap.set(4, 480)

    #model = YOLO("yolo-Weights/etonaaa.pt")

    #classNames = ["chair", "door", "table", "person"]

    model = YOLO("yolo-Weights/yolov8n.pt")

# object classes
    classNames = ["person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat",
              "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat",
              "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella",
              "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat",
              "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup",
              "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli",
              "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant", "bed",
              "diningtable", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone",
              "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors",
              "teddy bear", "hair drier", "toothbrush"
              ]
    while True:
        success, img = cap.read()
        results = model(img, stream=True)

        for r in results:
            boxes = r.boxes

            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

                cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 255), 3)

                confidence = math.ceil((box.conf[0] * 100)) / 100
                # print("Confidence --->", confidence)

                cls = int(box.cls[0])
                #print("Class name -->", classNames[cls])

                org = [x1, y1]
                font = cv2.FONT_HERSHEY_SIMPLEX
                fontScale = 1
                color = (255, 0, 0)
                thickness = 2

                cv2.putText(img, org, font, fontScale, color, thickness)

        cv2.imshow('Webcam', img)
        if cv2.waitKey(1) == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    # Create separate processes for lidar_buzzer and webcam functions
    #lidar_process = Process(target=lidar_buzzer)
    webcam_process = Process(target=webcam)

    # Start both processes
    #lidar_process.start()
    webcam_process.start()

    # Wait for both processes to finish
    #lidar_process.join()
    webcam_process.join()