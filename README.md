# CV-Arduino-Controller: Real-Time Gesture & UI Control

This project is a complete real-time control system that uses computer vision to translate hand gestures and interactive on-screen UI elements into physical hardware actions managed by an Arduino Uno.

## Key Features

-   **Real-Time Finger Counting:** Uses the MediaPipe library to detect one or two hands and accurately count the number of fingers held up (1-10). The count is instantly displayed on a physical 7-segment display.
-   **Gesture-Triggered Actions:** A specific finger count (e.g., 5 fingers) triggers a physical servo motor to rotate for a set duration.
-   **Interactive Virtual UI:** The application draws a virtual button on the video feed. "Touching" this button with your index finger toggles a physical LED on and off.
-   **Responsive & Non-Blocking:** The Arduino code is written using the `millis()` function for non-blocking timing, allowing the servo to operate without freezing the main loop or missing new commands for the display and LED.

## Technologies Used

-   **Computer Vision (Python):**
    -   `OpenCV` for camera capture and image processing.
    -   `MediaPipe` for robust hand and finger landmark detection.
    -   `PySerial` for communication between the Python script and the Arduino.
-   **Hardware & Microcontroller:**
    -   `Arduino Uno` (programmed in C++).
    -   7-Segment Display.
    -   SG90 Servo Motor.
    -   LEDs with current-limiting resistors.

## How It Works

1.  A Python script captures video from a webcam.
2.  MediaPipe processes each frame to detect hand landmarks.
3.  The script calculates the total number of fingers up and checks if an index finger is "touching" a virtual button's coordinates.
4.  Based on these inputs, it sends single-character commands (`'1'`, `'5'`, `'A'`, `'a'`, etc.) to the Arduino via the USB serial port.
5.  The Arduino listens for these commands and activates the appropriate hardware (display, servo, or LED) instantly.

## How to Run

1.  **Hardware Setup:** Assemble the Arduino circuit with the 7-segment display, servo motor (with external power), and LED.
2.  **Upload Arduino Code:** Upload the final `.ino` sketch to the Arduino Uno.
3.  **Install Python Libraries:**
    ```bash
    pip install opencv-python mediapipe pyserial
    ```
4.  **Configure Python Script:** Open the `.py` file and update the `PORT_NAME` variable to match your Arduino's COM port.
5.  **Run the Script:**
    ```bash
    python cv_to_arduino.py
    ```
