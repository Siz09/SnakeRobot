#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Servo driver
Adafruit_PWMServoDriver servoList = Adafruit_PWMServoDriver();
#define SERVOMIN 100 // Minimum pulse length count (calibrate according to your servos)
#define SERVOMAX 710 // Maximum pulse length count (calibrate according to your servos)

// Snake parameters
float pi = 3.14159;
int TotalNumberofServos = 7; // Adjust the number of servos as per your setup
float Shift = 2 * pi / TotalNumberofServos; // Phase lag between segments
float Wavelengths, rads;
int InteriorAngle, SetpointAngle, MaxAngleDisplacement;

// Ultrasonic sensor pins and variables
const int trig = 12; // Trigger pin
const int echo = 13; // Echo pin
int distance; 
long timee;
float velocity = 0.034; // Speed of sound in cm/µs (corrected)

void setup() {
  Serial.begin(115200); // Increased baud rate for ESP32
  servoList.begin();
  servoList.setPWMFreq(60);  // Set PWM frequency to 60 Hz for servos
  
  // Initialize snake to 90 degrees
  int pulselength = map(90, 0, 180, SERVOMIN, SERVOMAX); // 90 degrees center position
  for (int servo = 0; servo < TotalNumberofServos; servo++) {
    servoList.setPWM(servo, 0, pulselength);
    delay(15);
  }
  delay(1000);
  Serial.println("Servo initialization complete. Starting movement...");
  
  // Set ultrasonic sensor pins
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trig, LOW);
}

// Straight line configuration
void straightline() {
  Serial.println("Moving in straight line...");
  int pulselength = map(90, 0, 180, SERVOMIN, SERVOMAX); // 90 degrees
  for (int servo = 0; servo < TotalNumberofServos; servo++) {
    servoList.setPWM(servo, 0, pulselength);
    delay(100);
  }
}

// Ring configuration
void ring() {
  Serial.println("Forming ring configuration...");
  InteriorAngle = 180 - 360 / (TotalNumberofServos + 1); // Polygon formula
  SetpointAngle = abs(InteriorAngle - 90); // Offset from 90 degrees
  
  for (int servo = 0; servo < TotalNumberofServos; servo++) {
    int pulselength;
    if (servo % 2 == 0) {
      pulselength = map(SetpointAngle, 0, 180, SERVOMIN, SERVOMAX);
    } else {
      int angle = 90 + 90 - SetpointAngle;
      pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
    }
    servoList.setPWM(servo, 0, pulselength);
    delay(100);
  }
}

// Slithering motion
void slither(int offset, int Amplitude, int Speed, float Wavelengths) {
  MaxAngleDisplacement = abs(offset) + abs(Amplitude); // Max rotation calculation
  while (MaxAngleDisplacement > 90) { // Prevent out-of-range angles
    Amplitude = abs(Amplitude) - 1;
    MaxAngleDisplacement = abs(offset) + Amplitude;
    Serial.println("Amplitude reduced to prevent out-of-range angles.");
  }
  
  Serial.println("Starting slithering motion...");
  for (int i = 0; i < 360; i++) {
    rads = i * pi / 180.0; // Convert degrees to radians
    for (int servo = 0; servo < TotalNumberofServos; servo++) {
      int angle = 90 + offset + Amplitude * sin(Speed * rads + servo * Wavelengths * Shift);
      int pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
      servoList.setPWM(servo, 0, pulselength);
    }
    delay(10); // Adjust speed of slither motion
  }
}

// Static wave pattern
void staticWave(int offset, int Amplitude, float Wavelengths) {
  MaxAngleDisplacement = abs(offset) + abs(Amplitude); // Max rotation calculation
  while (MaxAngleDisplacement > 90) { // Prevent out-of-range angles
    Amplitude = abs(Amplitude) - 1;
    MaxAngleDisplacement = abs(offset) + Amplitude;
    Serial.println("Amplitude reduced to prevent out-of-range angles.");
  }

  Serial.println("Starting static wave pattern...");
  for (int servo = 0; servo < TotalNumberofServos; servo++) {
    int angle;
    if (servo % 2 == 0) {
      angle = 90 + offset + Amplitude * sin(servo * Wavelengths * Shift);
    } else {
      angle = 90 - offset - Amplitude * sin(servo * Wavelengths * Shift);
    }
    int pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
    servoList.setPWM(servo, 0, pulselength);
    delay(15);
  }
}

int readDistance() {
  // Read the distance from the ultrasonic sensor
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  timee = pulseIn(echo, HIGH);
  return (velocity * timee) / 2;
}

void loop() {
  // Read the distance from the sensor
  distance = readDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // If distance is too close (below a threshold), stop and change direction
  if (distance < 20) {  // You can adjust the threshold as needed
    Serial.println("Too close to wall! Changing direction.");
    // Change direction or stop snake, here we're using a basic stop
    straightline();  // You can change to another movement like "ring()" or "slither()" based on your needs
  } else {
    // Continue with the snake's current movement
    slither(0, 35, 3, 2);  // Adjust movement as needed
  }

  delay(1000);  // Delay to allow for smoother movement and sensor reading
}
