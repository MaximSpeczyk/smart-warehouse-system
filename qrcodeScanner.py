import cv2

# Set up camera object
cap = cv2.VideoCapture(0)

# QR code detector
detector = cv2.QRCodeDetector()

# Track last QR data
last_data = None

while True:
    _, img = cap.read()
    data, bbox, _ = detector.detectAndDecode(img)

    if bbox is not None:
        for i in range(len(bbox)):
            cv2.line(img, tuple(bbox[i][0]), tuple(bbox[(i + 1) % len(bbox)][0]),
                     color=(255, 0, 0), thickness=2)
        cv2.putText(img, data, (int(bbox[0][0][0]), int(bbox[0][0][1]) - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 250, 120), 2)

        if data and data != last_data:
            print("data found:", data)
            last_data = data
    else:
        # Reset last_data if no QR code is in view
        last_data = None

    cv2.imshow("code detector", img)

    if cv2.waitKey(1) == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
