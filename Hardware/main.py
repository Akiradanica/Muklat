from ultralytics import YOLO
import cv2
import math
import pyttsx3
from concurrent.futures import ThreadPoolExecutor
from multiprocessing import Process, Queue

engine = pyttsx3.init()

def calibrate_camera():
    # Example measurements
    known_distance = 2.5  # meters
    known_height = 1.7  # meters (height of the object, e.g., a person standing)

    # You would capture an image here and measure the pixel height of the known object.
    # This is a placeholder value for the height in pixels
    pixel_height = 250  # This needs to be measured

    # Calculate focal length
    focal_length = (pixel_height * known_distance) / known_height
    return focal_length

def speak(text):
    engine.say(text)
    engine.runAndWait()

# Function to determine if an object should be spoken
def should_speak(last_spoken, current_class):
    return last_spoken != current_class

# Placeholder function for distance estimation
def estimate_distance(box_height, focal_length, real_height):
    """
    Estimate the distance of an object based on its bounding box height in pixels,
    the camera's focal length, and the object's real height.
    :param box_height: The height of the object's bounding box in pixels
    :param focal_length: The focal length of the camera (in pixels)
    :param real_height: The real height of the object (in meters or any consistent unit)
    :return: Estimated distance to the object (in the same units as real_height)
    """
    # Simple distance estimation formula derived from similar triangles principle
    distance = (real_height * focal_length) / box_height
    return distance

def webcam(input_queue):
    cap = cv2.VideoCapture(0)
    cap.set(3, 640)  # Width
    cap.set(4, 480)  # Height

    model = YOLO("Hardware/yolo-Weights/etonaaa.pt")
    classNames = ["chair", "door", "table", "person"]
    last_spoken = None

    # Calibrate camera focal length
    focal_length = calibrate_camera()

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

                        # Example real heights in meters (or any consistent unit) for each class
                        # These are placeholders and should be adjusted based on real object sizes
                        real_heights = {"chair": 1, "door": 2, "table": 0.75, "person": 1.7}

                        cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 255), 3)
                        confidence = math.ceil((box.conf[0] * 100)) / 100
                        cls = int(box.cls[0])
                        class_name = classNames[cls]

                        # Estimate distance
                        real_height = real_heights.get(class_name, 0)  # Default to 1 meter if unknown object
                        distance = estimate_distance(box_height, focal_length, real_height)

                        org = (x1, y1)
                        cv2.putText(img, f'{class_name} {confidence:.2f}, Dist: {distance:.2f}m', org, cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

                        if should_speak(last_spoken, f"{class_name} {round(distance, 2)} meters away"):
                            executor.submit(speak, f"{class_name}, {round(distance, 2)} meters away")
                            last_spoken = f"{class_name} {round(distance, 2)} meters away"  # Update last spoken object

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
