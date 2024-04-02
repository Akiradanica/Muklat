from ultralytics import YOLO
import cv2
import math
import pyttsx3
from concurrent.futures import ThreadPoolExecutor
from multiprocessing import Process, Queue

engine = pyttsx3.init()

def speak(text):
    engine.say(text)
    engine.runAndWait()

# Function to determine if an object should be spoken
def should_speak(last_spoken, current_class):
    return last_spoken != current_class

def webcam(input_queue):
    cap = cv2.VideoCapture(0)
    cap.set(3, 640)  # Width
    cap.set(4, 480)  # Height

    model = YOLO("yolo-Weights/etonaaa.pt")
    classNames = ["chair", "door", "table", "person"]
    last_spoken = None

    with ThreadPoolExecutor(max_workers=4) as executor:
        try:
            while True:
                success, img = cap.read()
                if not success:
                    continue

                results = model(img, stream=True)

                for r in results:
                    boxes = r.boxes
                    for box in boxes:
                        x1, y1, x2, y2 = map(int, box.xyxy[0])

                        # Calculate box height (could also use width or area for different estimations)
                        box_height = y2 - y1


                        cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 255), 3)
                        confidence = math.ceil((box.conf[0] * 100)) / 100
                        cls = int(box.cls[0])
                        class_name = classNames[cls]


                        org = (x1, y1)
                        cv2.putText(img, f'{class_name},{confidence:.2f}, ', org, cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

                        if should_speak(last_spoken, f"{class_name} "):
                            executor.submit(speak, f"{class_name}")
                            last_spoken = f"{class_name}"  # Update last spoken object

                input_queue.put(img)  # Processed frame

                cv2.imshow('Webcam', img)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
        finally:
            cap.release()
            cv2.destroyAllWindows()

if __name__ == "__main__":
    frame_queue = Queue()
    webcam_process = Process(target=webcam, args=(frame_queue,))
    webcam_process.start()
    webcam_process.join()
