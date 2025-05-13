import cv2

# QRCode-Detektor erstellen
qr_detector = cv2.QRCodeDetector()

# Kamera öffnen (Kamera 0 = Pi Kamera)
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Kamera nicht gefunden.")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # QR-Code erkennen
    data, bbox, _ = qr_detector.detectAndDecode(frame)

    if bbox is not None:
        # Zeichne Umrandung
        for i in range(len(bbox)):
            pt1 = tuple(bbox[i][0])
            pt2 = tuple(bbox[(i + 1) % len(bbox)][0])
            cv2.line(frame, pt1, pt2, (255, 0, 0), 2)

        if data:
            print("QR-Code gefunden:", data)
            cv2.putText(frame, data, (20, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    # Live-Vorschau
    cv2.imshow("QR-Code Scanner", frame)

    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()