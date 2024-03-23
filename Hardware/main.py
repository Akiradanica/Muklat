from subprocess import call
from subprocess import Popen
from ultralytics import YOLO
import cv2
import math
from multiprocessing import Process, Queue 
 
def lidar_buzzer():
    call(["python", "lidar_buzzer.py"])
 
def webcam(input_queue):

    #settings for using esp32 cam
    # cap = cv2.VideoCapture('http://192.168.214.227:81/stream') 
    cap = cv2.VideoCapture(0)
    cap.set(3, 640)
    cap.set(4, 480)
 
    model = YOLO("yolo-Weights/etonaaa.pt")
 
    classNames = ["chair", "door", "table", "person"]
 
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
                print("Confidence --->", confidence)
 
                cls = int(box.cls[0])
                print("Class name -->", classNames[cls])
 
                org = [x1, y1]
                font = cv2.FONT_HERSHEY_SIMPLEX
                fontScale = 1
                color = (255, 0, 0)
                thickness = 2
 
                cv2.putText(img, classNames[cls], org, font, fontScale, color, thickness)
 
        input_queue.put(img)  # Put the processed frame into the queue
 
        cv2.imshow('Webcam', img)
        if cv2.waitKey(1) == ord('q'):
            break
 
    cap.release()
    cv2.destroyAllWindows()
 
if __name__ == "__main__":
    # Create a Queue for communication between processes
    frame_queue = Queue()
 
    # Create separate processes for lidar_buzzer and webcam functions
    lidar_process = Process(target=lidar_buzzer)
    webcam_process = Process(target=webcam, args=(frame_queue,))
 
    # Start both processes
    lidar_process.start()
    webcam_process.start()
 
    # Wait for both processes to finish
    lidar_process.join()
    webcam_process.join()