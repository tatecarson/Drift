#include <Servo.h>

Servo myservo;

int center = 90;      // middle angle
int amplitude = 30;   // how far to tilt each way (30° gives 60–120 range)
int pos = 0;
int step = 1;

void setup() {
  myservo.attach(9);
  pos = center - amplitude;  // start at one end of the seesaw
}

void loop() {
  myservo.write(pos);
  pos += step;

  // turn around when we reach either end of the seesaw range
  if (pos >= center + amplitude || pos <= center - amplitude) {
    step = -step;
    delay(300); 
  }

  delay(8);  // adjust for speed: bigger = slower, smaller = faster
}
