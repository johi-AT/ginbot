#include <main.h>
#include <Arduino.h>

const byte pin_button = 16;

// { pin, dispense duration, _ )
fluid fluids[] = {
  {11, 2000, 0},
  {12, 3000, 0},
  {13, 5000, 0},
};
const int INTERVAL = 100; // process every x ms

// variables
stateEnum state;
unsigned long time_start, time_current;
int time_elapsed;
unsigned long last_step;
bool all_zero;

void setup() {
  Serial.begin(115200);
  pinMode(pin_button, INPUT_PULLUP);

  for(fluid fl : fluids) {
    pinMode(fl.pin, OUTPUT);
  }

  state = READY;
}

void loop() {
  switch (state) {
  case READY:
    if (digitalRead(pin_button) == HIGH) return;

    // Button was pressed and we are ready
   
    for(fluid &fl : fluids) {
      fl.time_left_ms = fl.time_pour;
    }
    
    //Serial.print("WTF: ");
    //Serial.print(fluids[0].time_left_ms);

    time_start = millis();
    state = SERVING;
    break;
  case SERVING:
    time_current = millis();
    time_elapsed = time_current - time_start;
    time_start = time_current;

    all_zero = true;
    for(fluid &fl : fluids) {
      fl.time_left_ms = max(0, fl.time_left_ms - time_elapsed);

      if (fl.time_left_ms > 0) {
        digitalWrite(fl.pin, HIGH);
        all_zero = false;
      } else {
        digitalWrite(fl.pin, LOW);
      }

    }

    if ( time_current - last_step >= INTERVAL ) {
      Serial.print("time_left_ms: ");
      for(fluid fl : fluids) {
        Serial.print(fl.time_left_ms);
        Serial.print(" - ");
      }
      Serial.println();
      last_step = millis();
    }
    
    if (all_zero == true) {
      state = READY;
      Serial.println("READY");
    }
    break;
  }
}
