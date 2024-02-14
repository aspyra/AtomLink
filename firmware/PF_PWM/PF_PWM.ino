//#define debug
#include "AtomLink-PFsniffer.h"
#include <Servo.h>

Servo servo = Servo();

const int servoPin = 25;

void setup() {
  Serial.begin(115200);  // setup serial communication at 115200 baud
  PFsniffer_setup();
}

void loop() {
  int16_t sgnd = PFsniffer_get();
  float newPosition = (sgnd+minMaxVal)*(90.0/minMaxVal);
#ifdef debug
  Serial.print("Position: ");
  Serial.println(newPosition);
#endif
  servo.write(servoPin, newPosition);
  delay(6);
}