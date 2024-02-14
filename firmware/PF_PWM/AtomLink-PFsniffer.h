#define minMaxVal 4096
#define rtnType int16_t //adjust to fit +/- minMaxVal

typedef volatile struct{
  unsigned long lastRisingEdge = 0;
  unsigned long lastFallingEdge = 0;
  unsigned long highTime = 0;
  unsigned long lowTime = 0;
  unsigned int pulseCounter = 0;
  unsigned long dutyCycle = 0;   //Stores duty cycle of PWM signal on M1 contact
  unsigned long lastUpdate = 0;  //Stores last refresh time of M1 contact reading
} PfIsrStruct;

PfIsrStruct M1data, M2data;

volatile SemaphoreHandle_t M1semaphore;
volatile SemaphoreHandle_t M2semaphore;
const byte M1Pin = 21;
const byte M2Pin = 22;



void PfIsr(volatile SemaphoreHandle_t & semaphore, const byte pin, PfIsrStruct & data) {
  if (pdTRUE == xSemaphoreTakeFromISR(semaphore, NULL)) {
    if (digitalRead(pin) == HIGH) {
      // rising edge, remember the time
      data.lastRisingEdge = micros();
      if (data.lastFallingEdge > 0) {
        data.lowTime += (data.lastRisingEdge - data.lastFallingEdge);  // complete low time
      }
    } else {
      // falling edge, remember the time
      data.lastFallingEdge = micros();
      if (data.lastRisingEdge > 0) {
        data.highTime += (data.lastFallingEdge - data.lastRisingEdge);  // complete high time
      }

      data.pulseCounter++;
      if (data.pulseCounter == 10) {
        data.dutyCycle = (data.highTime * minMaxVal) / (data.lowTime + data.highTime);  // calculate the duty cycle
        data.lastUpdate = millis();
        data.pulseCounter = 0;   // reset the counter
        data.highTime = 0;  // reset high time
        data.lowTime = 0;   // reset low time
      }
    }
    xSemaphoreGiveFromISR(semaphore, NULL);
  }
}

void IRAM_ATTR M1Interrupt() {
  PfIsr(M1semaphore, M1Pin, M1data);
}

void IRAM_ATTR M2Interrupt() {
  PfIsr(M2semaphore, M2Pin, M2data);
}

void PFsniffer_setup(){
  pinMode(M1Pin, INPUT_PULLUP);  // set the pin to input
  pinMode(M2Pin, INPUT_PULLUP);  // set the pin to input
  M1semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(M1semaphore);
  M2semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(M2semaphore);
  attachInterrupt(digitalPinToInterrupt(M1Pin), M1Interrupt, CHANGE);  // attach the interrupt
  attachInterrupt(digitalPinToInterrupt(M2Pin), M2Interrupt, CHANGE);  // attach the interrupt
}

rtnType PFsniffer_get(){
  unsigned long dc1 = 0, ud1 = 0, dc2 = 0, ud2 = 0;
  bool m1 = false, m2 = false;
  if (pdTRUE == xSemaphoreTake(M1semaphore, 5)) {
    dc1 = M1data.dutyCycle;
    ud1 = M1data.lastUpdate;
    m1 = digitalRead(M1Pin);
    xSemaphoreGive(M1semaphore);
  }
  if (pdTRUE == xSemaphoreTake(M2semaphore, 5)) {
    dc2 = M2data.dutyCycle;
    ud2 = M2data.lastUpdate;
    m2 = digitalRead(M2Pin);
    xSemaphoreGive(M2semaphore);
  }

#ifdef debug
  Serial.print("DC1(");
  Serial.print(dc1);
  Serial.print(") UD1(");
  Serial.print(ud1);
  Serial.print(") M1(");
  Serial.print(m1);
  Serial.print(") DC2(");
  Serial.print(dc2);
  Serial.print(") UD2(");
  Serial.print(ud2);
  Serial.print(") M2(");
  Serial.print(m2);
  Serial.print(") millis(");
  Serial.print(millis());
#endif

  //MouldKing PWM is ~500kHz -> rising edges 2ms apart.
  //If for 5ms there is no rising edge, then this channel is constantly on or off.
  //Set max value; direction based on pin readings.
  if (millis() - ud1 > 20) {
      dc1 = m1 * minMaxVal;
  }
  if (millis() - ud2 > 20) {
      dc2 = m2 * minMaxVal;
  }
#ifdef debug
  Serial.print(") return: ");
  Serial.println(rtnType(dc1 - dc2));
#endif
  return (dc1 - dc2);
}