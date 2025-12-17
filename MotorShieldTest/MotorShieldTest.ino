#include <Servo.h>
#include <math.h>

Servo servo1;

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

void setup() {
  servo1.attach(9);
}

void loop() {
  moveServoWithEase(servo1, 0, 110, 2000);
  delay(1000);

  moveServoWithEase(servo1, 110, 0, 2000);
  delay(1000);
}
