// =========================================================================
// ===        FINAL ARDUINO CODE FOR ALL FEATURES (YOUR WIRING)        ===
// =========================================================================
// This code controls:
// 1. The 7-Segment Display (based on your specific pinout)
// 2. The Servo Motor (non-blocking)
// 3. One LED

#include <Servo.h>  // Include the library for the servo motor

// --- GLOBAL VARIABLES ---
char input;         // Stores the character received from Python
Servo myServo;      // Creates a servo object

// --- HARDWARE PINS ---
const int led1Pin = 2; // Pin for the single LED
// Your exact pin setup for the 7-segment display - UNCHANGED
const int pin_g = 4;
const int pin_f = 6;
const int pin_a = 9;
const int pin_b = 10;
const int pin_e = 5;
const int pin_d = 3;
const int pin_c = 12;

// --- NON-BLOCKING TIMER VARIABLES ---
int servoState = 0;              // 0=idle, 1=moving to 90, 2=waiting to return
unsigned long servoTimer = 0;    // Stores the time when the servo starts waiting
const long ROTATION_TIME = 4000; // The time the servo stays rotated (4 seconds)

// =========================================================================
// ===                          SETUP                                    ===
// =========================================================================
void setup() {
  Serial.begin(9600); // Start serial communication

  // Setup 7-segment display pins as outputs
  pinMode(pin_g, OUTPUT);
  pinMode(pin_f, OUTPUT);
  pinMode(pin_a, OUTPUT);
  pinMode(pin_b, OUTPUT);
  pinMode(pin_e, OUTPUT);
  pinMode(pin_d, OUTPUT);
  pinMode(pin_c, OUTPUT);

  // Setup the LED pin as an output
  pinMode(led1Pin, OUTPUT);

  // Setup the servo
  myServo.attach(7); // Attach the servo to pin 7
  myServo.write(0);  // Start the servo at 0 degrees
}

// =========================================================================
// ===                        MAIN LOOP                                  ===
// =========================================================================
void loop() {
  // This loop runs continuously, thousands of times per second

  // PART 1: Check for any new command from the Python script
  if (Serial.available() > 0) {
    input = Serial.read();

    // Is it a number for the display?
    if (input >= '0' && input <= '9') {
      clearDisplay();
      displayDigit(input);
    } 
    // Or is it a command for the LED?
    else {
      handleLedCommands(input);
    }
  }

  // PART 2: Always check and update the servo state (non-blocking)
  handleServo(); 
}

// =========================================================================
// ===                  COMMAND HANDLING FUNCTIONS                       ===
// =========================================================================

// This function handles the commands for the LED
void handleLedCommands(char command) {
  switch (command) {
    case 'A': digitalWrite(led1Pin, HIGH); break; // Turn LED ON
    case 'a': digitalWrite(led1Pin, LOW);  break; // Turn LED OFF
  }
}

// This function handles the non-blocking servo movement
void handleServo() {
  // If the servo is in state 1, move it and start the timer
  if (servoState == 1) {
    myServo.write(90);       // Move to 90 degrees
    servoTimer = millis();   // Record the current time (start the 4-second timer)
    servoState = 2;          // Change state to "waiting"
  }

  // If the servo is in state 2, check if 4 seconds have passed
  if (servoState == 2 && millis() - servoTimer >= ROTATION_TIME) {
    myServo.write(0);        // If time is up, move back to 0
    servoState = 0;          // Reset to the idle state, ready for the next "5"
  }
}

// This function decides which display function to call and triggers the servo
void displayDigit(char digit) {
  switch (digit) {
    case '1': show1(); break;
    case '2': show2(); break;
    case '3': show3(); break;
    case '4': show4(); break;
    case '5':
      show5(); // Show the number 5
      if (servoState == 0) { // If servo is idle...
        servoState = 1;      // ...start the servo sequence
      }
      break;
    case '6': show6(); break;
    case '7': show7(); break;
    case '8': show8(); break;
    case '9': show9(); break;
    default: clearDisplay(); break;
  }
}

// =========================================================================
// ===         YOUR 7-SEGMENT DISPLAY FUNCTIONS (UNCHANGED)            ===
// =========================================================================

void clearDisplay() {
  digitalWrite(pin_g, LOW); digitalWrite(pin_f, LOW); digitalWrite(pin_a, LOW);
  digitalWrite(pin_b, LOW); digitalWrite(pin_e, LOW); digitalWrite(pin_d, LOW);
  digitalWrite(pin_c, LOW);
}
void show1() { digitalWrite(pin_b, HIGH); digitalWrite(pin_c, HIGH); }
void show2() { digitalWrite(pin_a, HIGH); digitalWrite(pin_b, HIGH); digitalWrite(pin_g, HIGH); digitalWrite(pin_e, HIGH); digitalWrite(pin_d, HIGH); }
void show3() { digitalWrite(pin_a, HIGH); digitalWrite(pin_b, HIGH); digitalWrite(pin_g, HIGH); digitalWrite(pin_c, HIGH); digitalWrite(pin_d, HIGH); }
void show4() { digitalWrite(pin_f, HIGH); digitalWrite(pin_g, HIGH); digitalWrite(pin_b, HIGH); digitalWrite(pin_c, HIGH); }
void show5() { digitalWrite(pin_a, HIGH); digitalWrite(pin_f, HIGH); digitalWrite(pin_g, HIGH); digitalWrite(pin_c, HIGH); digitalWrite(pin_d, HIGH); }
void show6() { digitalWrite(pin_a, HIGH); digitalWrite(pin_f, HIGH); digitalWrite(pin_g, HIGH); digitalWrite(pin_e, HIGH); digitalWrite(pin_c, HIGH); digitalWrite(pin_d, HIGH); }
void show7() { digitalWrite(pin_a, HIGH); digitalWrite(pin_b, HIGH); digitalWrite(pin_c, HIGH); }
void show8() { digitalWrite(pin_a, HIGH); digitalWrite(pin_b, HIGH); digitalWrite(pin_c, HIGH); digitalWrite(pin_d, HIGH); digitalWrite(pin_e, HIGH); digitalWrite(pin_f, HIGH); digitalWrite(pin_g, HIGH); }
void show9() { digitalWrite(pin_a, HIGH); digitalWrite(pin_b, HIGH); digitalWrite(pin_g, HIGH); digitalWrite(pin_f, HIGH); digitalWrite(pin_c, HIGH); digitalWrite(pin_d, HIGH); }
