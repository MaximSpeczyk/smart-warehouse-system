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

print("Starte Scan. Drücke Strg + C um zu stoppen")

try:
    while True:
        # Schritt 1: Bild aufnehmen
        subprocess.run(["libcamera-still", "-n", "-t", "100", "-o", image_path],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        # Schritt 2: Bild laden
        image = cv2.imread(image_path)

        # Schritt 3: QR-Codes dekodieren
        decoded_objects = decode(image)

        for obj in decoded_objects:
            qr_data = obj.data.decode('utf-8')

            # Immer den erkannten Code ausgeben
            print(f"Erkannter QR Code: {qr_data}")

            # Nur senden, wenn der Code ≤ 4 Zeichen lang ist
            if len(qr_data) <= 4:
                client.publish(MQTT_TOPIC, qr_data)
                print(f"✅ Gesendet zu MQTT ({MQTT_TOPIC}): {qr_data}")

        time.sleep(0.5)

except KeyboardInterrupt:
    print("\nScanner gestoppt.")
    client.disconnect()
