#include <Servo.h>
#include <math.h>

// Pan and Tilt Servos
Servo servoPan;   // Pin 9 - horizontal panning
Servo servoTilt;  // Pin 10 - vertical tilting bracket

// Range constants
const int PAN_MIN = 0;
const int PAN_MAX = 180;
const int TILT_MIN = 0;   // Prevent tilting too far down
const int TILT_MAX = 180;  // Prevent tilting too far up

// Note: Tilt servo now uses direct physical degrees (0–180)

// --------------------------
// EASING PRESETS
// --------------------------

// Slow start → fast end
float logEaseIn(float t) {
  return log(1 + 9 * t) / log(10); 
}

// Fast start → slow end
float logEaseOut(float t) {
  float inv = 1.0 - t;
  return 1.0 - (log(1 + 9 * inv) / log(10));
}

// Slow → fast → slow (smoother)
float logEaseInOut(float t) {
  if (t < 0.5) {
    float tt = t * 2.0 ;
    return 0.5 * (log(1 + 9 * tt) / log(10));
  } else {
    float tt = (t - 0.5) * 2.0;
    float eased = log(1 + 9 * tt) / log(10);
    return 0.5 + 0.5 * eased;
  }
}

// No easing
float linearEase(float t) {
  return t;
}

// --------------------------
// SELECT YOUR PRESET HERE
// --------------------------

// Options: logEaseIn, logEaseOut, logEaseInOut, linearEase
float (*ease)(float) = logEaseIn;   // <—— change this to switch preset

// --------------------------

// --------------------------
// TEST CONFIG: choose which single servo to exercise
// --------------------------
enum TestTarget { TEST_PAN, TEST_TILT };
const TestTarget TEST_TARGET = TEST_TILT; // change to TEST_TILT to test tilt only

// --------------------------

void moveServoWithEase(Servo &s, int startPos, int endPos, int durationMs) {
  const int steps = 100;
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps; 
    float eased = ease(t);       // use selected easing preset

    int pos = startPos + (endPos - startPos) * eased;
    s.write(pos);

    delay(durationMs / steps);
  }
}

// Convenience wrappers to move a single named servo with clamped ranges
void movePanWithEase(int startPos, int endPos, int durationMs) {
  startPos = constrain(startPos, PAN_MIN, PAN_MAX);
  endPos   = constrain(endPos, PAN_MIN, PAN_MAX);
  moveServoWithEase(servoPan, startPos, endPos, durationMs);
}

void moveTiltWithEase(int startPos, int endPos, int durationMs) {
  startPos = constrain(startPos, TILT_MIN, TILT_MAX);
  endPos   = constrain(endPos, TILT_MIN, TILT_MAX);
  moveServoWithEase(servoTilt, startPos, endPos, durationMs);
}

// Move both pan and tilt servos simultaneously with coordinated motion
void moveDualServosWithEase(int panStart, int panEnd, int tiltStart, int tiltEnd, int durationMs) {
  const int steps = 100;
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;
    float eased = ease(t);

    // Calculate positions for both servos
    int panPos = panStart + (panEnd - panStart) * eased;
    int tiltPos = tiltStart + (tiltEnd - tiltStart) * eased;

    // Constrain to valid ranges
    panPos = constrain(panPos, PAN_MIN, PAN_MAX);
    tiltPos = constrain(tiltPos, TILT_MIN, TILT_MAX);

    // Move both servos
    servoPan.write(panPos);
    servoTilt.write(tiltPos);

    delay(durationMs / steps);
  }
}

void setup() {
  servoPan.attach(9);    // Pan servo on pin 9
  servoTilt.attach(10);  // Tilt servo on pin 10
  Serial.begin(115200);
  delay(50);
}

void loop() {
  // if (TEST_TARGET == TEST_PAN) {
  //   // Test pan only
  //   movePanWithEase(30, 150, 1200);
  //   delay(500);
  //   movePanWithEase(150, 30, 1200);
  //   delay(800);
  // } else {
  //   // Test tilt only
  //   Serial.println("Tilt: moving towards higher degrees");
  //   moveTiltWithEase(0, 90, 1200);
  //   delay(500);
  // //   Serial.println("Tilt: moving towards lower degrees");
  //   moveTiltWithEase(90, 0, 1200);
  //   delay(800);
    Serial.println("Moving pan and tilt together");
    moveDualServosWithEase(30, 150, 0, 90, 1200);
    delay(500);
    moveDualServosWithEase(150, 30, 90, 0, 1200);
    delay(800);
  }

