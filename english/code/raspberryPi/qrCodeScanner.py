import subprocess
import cv2
from pyzbar.pyzbar import decode
import time
import paho.mqtt.client as mqtt

# MQTT setup
MQTT_BROKER = "192.168.63.230" 
MQTT_PORT = 1883
MQTT_TOPIC = "QRCODE"

client = mqtt.Client(protocol=mqtt.MQTTv311)
client.connect(MQTT_BROKER, MQTT_PORT, 60)

image_path = "capture.jpg"

print("Starting scan. Press Ctrl + C to stop.")

try:
    while True:
        # Step 1: Capture an image
        subprocess.run(["libcamera-still", "-n", "-t", "100", "-o", image_path],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        # Step 2: Load the image
        image = cv2.imread(image_path)

        # Step 3: Decode QR codes
        decoded_objects = decode(image)

        for obj in decoded_objects:
            qr_data = obj.data.decode('utf-8')

            # Always print the detected code
            print(f"Detected QR Code: {qr_data}")

            # Only send if code is 4 characters or less
            if len(qr_data) <= 4:
                client.publish(MQTT_TOPIC, qr_data)
                print(f"✅ Sent to MQTT ({MQTT_TOPIC}): {qr_data}")

        time.sleep(0.5)

except KeyboardInterrupt:
    print("\nScanner stopped.")
    client.disconnect()