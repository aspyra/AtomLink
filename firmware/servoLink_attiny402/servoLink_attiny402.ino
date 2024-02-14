#include <Servo_megaTinyCore.h>
/* AtomLink Mini v1.0 (Attiny402) pinout
 *  PF_M1 -> PA2/D3
 *  PF_M2 -> PA6/D0
 *  Servo -> PA3/D4
 *  Button-> PA7/D1
 *  LED   -> PA1/D2
 *  
  */
#define pin_LED 2
#define pin_BTN 1
#define pin_M1 3
#define pin_M2 0
#define pin_SRV 4

Servo srv;

int pos = 0;    // variable to store the servo position

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pin_BTN, INPUT_PULLUP);
  pinMode(pin_M1, INPUT_PULLUP);
  pinMode(pin_M2, INPUT_PULLUP);
  pinMode(pin_LED, OUTPUT);
  digitalWrite(pin_LED, HIGH);
  pinMode(pin_SRV, OUTPUT);
  digitalWrite(pin_SRV, LOW);
  srv.attach(pin_SRV);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(pin_LED, digitalRead(pin_BTN));
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    srv.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    srv.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
}
