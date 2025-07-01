import cv2
import mediapipe as mp
import serial
import time

# =========================================================================
# ===                  SETUP & CONNECTION                             ===
# =========================================================================
# IMPORTANT: Confirm this matches your Arduino's port (e.g., 'COM5')
PORT_NAME = 'COM5' 

try:
    arduino = serial.Serial(port=PORT_NAME, baudrate=9600, timeout=0.1)
    print(f"Successfully connected to Arduino on {PORT_NAME}")
    time.sleep(2) # Give Arduino time to reset
except serial.SerialException as e:
    print(f"FATAL ERROR: Could not connect to Arduino on {PORT_TNAME}.")
    print("Please check the port name and ensure the Arduino is plugged in.")
    exit()

# =========================================================================
# ===                 MEDIAPIPE & CV SETUP                            ===
# =========================================================================
mp_hands = mp.solutions.hands
# Set to detect a maximum of 2 hands
hands = mp_hands.Hands(max_num_hands=2, min_detection_confidence=0.7)
mp_draw = mp.solutions.drawing_utils
# Landmark IDs for finger tips
tip_ids = [4, 8, 12, 16, 20]
INDEX_FINGER_TIP = 8 # Specific landmark for the cursor

# =========================================================================
# ===                 VIRTUAL BUTTON SETUP                            ===
# =========================================================================
# A single button on the bottom-left of the screen
button_pos = (50, 380)
button_size = (150, 70)
button_on = False
# This flag prevents the button from flickering rapidly when touched
button_pressed_flag = False 

# =========================================================================
# ===                 MAIN PROGRAM                                    ===
# =========================================================================
# Start webcam using the reliable DSHOW backend
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
last_sent_count = -1 # Keep track of last finger count sent

# Helper function to check if the cursor is inside the button's area
def is_touching(cursor, button_pos, button_size):
    bx, by = button_pos
    bw, bh = button_size
    cx, cy = cursor
    return bx < cx < bx + bw and by < cy < by + bh

print("\nStarting camera... Show fingers or touch the button. Press 'q' to quit.")

while True:
    success, img = cap.read()
    if not success:
        continue
    
    # Flip the image for a mirror-like view and get its dimensions
    img = cv2.flip(img, 1)
    h, w, c = img.shape

    # --- 1. DRAW THE VIRTUAL BUTTON ---
    # Change color based on its ON/OFF state
    button_color = (0, 255, 0) if button_on else (0, 0, 255) # Green for ON, Red for OFF
    cv2.rectangle(img, button_pos, (button_pos[0] + button_size[0], button_pos[1] + button_size[1]), button_color, cv2.FILLED)
    cv2.putText(img, "LED 1", (button_pos[0] + 30, button_pos[1] + 45), cv2.FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3)

    # --- 2. PROCESS HANDS ---
    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    results = hands.process(img_rgb)
    
    total_finger_count = 0
    if results.multi_hand_landmarks:
        # Loop through each hand detected in the frame
        for hand_landmarks in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(img, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            
            # --- 2a. BUTTON TOUCH LOGIC ---
            # Get index finger tip coordinates in pixels
            index_tip_lm = hand_landmarks.landmark[INDEX_FINGER_TIP]
            cursor = (int(index_tip_lm.x * w), int(index_tip_lm.y * h))
            cv2.circle(img, cursor, 10, (255, 0, 255), cv2.FILLED) # Draw a cursor

            # Check if the cursor is touching the button
            if is_touching(cursor, button_pos, button_size):
                # Only trigger if this is the FIRST frame we are touching
                if not button_pressed_flag:
                    button_on = not button_on # Toggle the state ON/OFF
                    # Prepare the command to send to Arduino
                    command = b'A' if button_on else b'a'
                    arduino.write(command)
                    print(f"Button Touched! Sent command: {command.decode()}")
                    button_pressed_flag = True # Set the flag to prevent re-triggering
            else:
                button_pressed_flag = False # Reset the flag when the finger leaves the button

            # --- 2b. FINGER COUNTING LOGIC ---
            fingers_on_this_hand = []
            # Thumb (checks horizontal position)
            if hand_landmarks.landmark[tip_ids[0]].x < hand_landmarks.landmark[tip_ids[0] - 1].x:
                fingers_on_this_hand.append(1)
            else:
                fingers_on_this_hand.append(0)

            # Other 4 fingers (checks vertical position)
            for id in range(1, 5):
                if hand_landmarks.landmark[tip_ids[id]].y < hand_landmarks.landmark[tip_ids[id] - 2].y:
                    fingers_on_this_hand.append(1)
                else:
                    fingers_on_this_hand.append(0)
            
            # Add the count of this hand to the grand total
            total_finger_count += fingers_on_this_hand.count(1)

    # --- 3. SEND FINGER COUNT DATA ---
    # Only send if the finger count has changed
    if total_finger_count != last_sent_count:
        # Convert the number to a string character (e.g., 7 -> '7') and send it
        arduino.write(str(total_finger_count).encode())
        print(f"Finger count changed. Sent: {total_finger_count}")
        last_sent_count = total_finger_count

    # --- 4. DISPLAY EVERYTHING ---
    cv2.putText(img, f"Sending: {total_finger_count}", (20, 70), cv2.FONT_HERSHEY_PLAIN, 3, (0, 255, 0), 3)
    cv2.imshow("Full Control Panel", img)
    
    # Check for quit command ('q' key)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# --- 5. CLEANUP ---
print("Closing program...")
arduino.write(b'0') # Clear the 7-segment display
arduino.write(b'a') # Ensure the LED is off
arduino.close()     # Close the serial connection
cap.release()
cv2.destroyAllWindows()
