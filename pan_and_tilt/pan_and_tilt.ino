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

// --------------------------
// TILT MAPPING: servo mounted on its side
// --------------------------
// If the tilt servo's mechanical orientation inverts left/right, set to true.
const bool TILT_INVERT = false;

// Define a logical left→right coordinate for tilt: 0 = far left, 180 = far right.
// We will only use the RIGHT side to avoid hitting the pan servo.
// Adjust these two values to match your hardware clearance.
const int TILT_RIGHT_SAFE_LOGICAL_MIN = 95;   // just to the right of center
const int TILT_RIGHT_SAFE_LOGICAL_MAX = 165;  // far right, but before interference

// Named, meaningful tilt positions on the right side
enum TiltPose { TILT_RIGHT_NEAR, TILT_RIGHT_MID, TILT_RIGHT_FAR };

// Map pose → logical degrees (left→right space)
int tiltPoseToLogical(TiltPose p) {
  switch (p) {
    case TILT_RIGHT_NEAR: return TILT_RIGHT_SAFE_LOGICAL_MIN;          // closest to center
    case TILT_RIGHT_MID:  return (TILT_RIGHT_SAFE_LOGICAL_MIN + TILT_RIGHT_SAFE_LOGICAL_MAX) / 2;
    case TILT_RIGHT_FAR:  return TILT_RIGHT_SAFE_LOGICAL_MAX;          // far right
  }
  return TILT_RIGHT_SAFE_LOGICAL_MIN;
}

// Convert logical left→right degrees to actual servo angle, respecting inversion
int tiltLogicalToServoAngle(int logicalDeg) {
  logicalDeg = constrain(logicalDeg, 0, 180);
  return TILT_INVERT ? (180 - logicalDeg) : logicalDeg;
}

// Convenience: move between named right-side poses with easing and right-side safety
void moveTiltPoseWithEase(TiltPose startPose, TiltPose endPose, int durationMs) {
  int startLogical = constrain(tiltPoseToLogical(startPose), TILT_RIGHT_SAFE_LOGICAL_MIN, TILT_RIGHT_SAFE_LOGICAL_MAX);
  int endLogical   = constrain(tiltPoseToLogical(endPose),   TILT_RIGHT_SAFE_LOGICAL_MIN, TILT_RIGHT_SAFE_LOGICAL_MAX);
  int startServo   = tiltLogicalToServoAngle(startLogical);
  int endServo     = tiltLogicalToServoAngle(endLogical);
  moveServoWithEase(servoTilt, startServo, endServo, durationMs);
}

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
const TestTarget TEST_TARGET = TEST_PAN; // change to TEST_TILT to test tilt only

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
}

void loop() {
  if (TEST_TARGET == TEST_PAN) {
    // Test pan only
    movePanWithEase(30, 150, 1200);
    delay(500);
    movePanWithEase(150, 30, 1200);
    delay(800);
  } else {
    // Test tilt only
    moveTiltPoseWithEase(TILT_RIGHT_NEAR, TILT_RIGHT_FAR, 1200);
    delay(500);
    moveTiltPoseWithEase(TILT_RIGHT_FAR, TILT_RIGHT_NEAR, 1200);
    delay(800);
  }
}
