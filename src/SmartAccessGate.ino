#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Define the pins for the ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

// Define the pin for the servo motor
Servo gateServo;

// Define the distance threshold (adjusted to 8 cm)
const int distanceThreshold = 8; // 8 cm

// Timing variables
unsigned long lastDetectionTime = 0;      // Store the last time an object was detected
const unsigned long noObjectDelay = 6000; // Wait 6 seconds without detection before closing
const unsigned long warningTime = 1000;   // Buzzer warning time (1 second)

// Gate position control variables
int currentAngle = 0;    // Current angle of the gate (starts closed)
int openAngle = 90;      // The angle for the gate to be fully open
int closeAngle = 0;      // The angle for the gate to be fully closed
int angleStep = 30;      // Step size for each cycle (30 degrees)

// LEDs
const int greenLEDPin = 7;   // Pin for the green LED (Gate opening)
const int redLEDPin = 8;     // Pin for the red LED (Gate closing)

// Buzzer
const int buzzerPin = 13;    // Pin for the buzzer

// Switch for manual override
const int switchPin = A0;    // Pin for the manual switch

// Manual mode flag
bool isManualOverrideActive = false;  // Tracks whether the manual override is active

// Initialize the I2C LCD display (replace 0x27 with the correct address if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize the servo and set it to the closed position
  gateServo.attach(6);           // Attach the servo to pin 6
  gateServo.write(currentAngle);  // Start with gate closed (0 degrees)

  // Initialize the ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize the LEDs
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  digitalWrite(greenLEDPin, LOW); // Initially OFF
  digitalWrite(redLEDPin, LOW);   // Initially OFF

  // Initialize the buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);   // Buzzer OFF initially

  // Initialize the switch
  pinMode(switchPin, INPUT_PULLUP); // Use internal pull-up resistor

  // Initialize the LCD
  lcd.init();          // Initialize the LCD
  lcd.backlight();     // Turn on the backlight
  lcd.clear();
  lcd.print("System Ready"); // Initial message
  delay(2000);
  lcd.clear();
}

void loop() {
  // Check if manual switch is pressed
  if (digitalRead(switchPin) == LOW) {
    manualGateControl();  // Handle the manual override mode
  } else {
    if (!isManualOverrideActive) {
      automaticGateControl();  // Continue in automatic mode if manual override is inactive
    }
  }

  delay(100); // Short delay before the next reading
}

// Function for manual gate opening and override
void manualGateControl() {
  // Toggle the manual override flag on button press
  if (!isManualOverrideActive) {
    isManualOverrideActive = true;
    openGate();  // Open the gate on the first button press
  } else {
    isManualOverrideActive = false;
    closeGate(); // Close the gate on the second button press
  }

  // Wait for the button to be released to avoid repeated triggering
  while (digitalRead(switchPin) == LOW) {
    delay(100);  // Small delay
  }
}

// Function for automatic gate control
void automaticGateControl() {
  // Calculate distance using the ultrasonic sensor
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert to cm

  // If the car is detected within the threshold (8 cm or less)
  if (distance <= distanceThreshold) {
    // If the gate is currently closed, initiate the open process
    if (currentAngle == closeAngle) {
      // Sound the buzzer for 1 second before opening
      digitalWrite(buzzerPin, HIGH);
      delay(warningTime);
      digitalWrite(buzzerPin, LOW);

      // Display "Opening Gate!" while the green LED blinks
      lcd.clear();
      lcd.print("Opening Gate!");
      blinkLED(greenLEDPin, 3000); // Blink green LED for 3 seconds

      openGate();  // Open the gate immediately after LED blinking
    }

    // Reset the last detection time to the current time
    lastDetectionTime = millis();
  }

  // Check if no object has been detected for more than 6 seconds
  if (millis() - lastDetectionTime > noObjectDelay && currentAngle == openAngle) {
    // Sound the buzzer for 3 seconds before closing
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);

    // Display "Closing Gate!" while the red LED blinks
    lcd.clear();
    lcd.print("Closing Gate!");
    blinkLED(redLEDPin, 3000); // Blink red LED for 3 seconds

    closeGate();  // Close the gate after the LED blinks
  }
}

// Function to blink an LED for a given duration (in milliseconds)
void blinkLED(int ledPin, int duration) {
  unsigned long blinkStartTime = millis();
  while (millis() - blinkStartTime < duration) {
    digitalWrite(ledPin, HIGH);   // Turn LED ON
    delay(250);                   // Wait 250ms
    digitalWrite(ledPin, LOW);    // Turn LED OFF
    delay(250);                   // Wait 250ms
  }
}

// Function to open the gate smoothly (30-degree increments)
void openGate() {
  for (int angle = currentAngle; angle <= openAngle; angle += angleStep) {
    gateServo.write(angle);
    delay(500);  // Small delay for smooth movement
  }
  currentAngle = openAngle;
}

// Function to close the gate smoothly (30-degree increments)
void closeGate() {
  for (int angle = currentAngle; angle >= closeAngle; angle -= angleStep) {
    gateServo.write(angle);
    delay(500);  // Small delay for smooth movement
  }
  currentAngle = closeAngle;
}
