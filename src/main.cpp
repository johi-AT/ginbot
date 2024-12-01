#include <main.h>
#include <Arduino.h>

const byte PIN_BUTTON = 16;
const byte PIN_SWITCH = 18;

const byte RECIPE_GIN_TONIC = 0x3;    // 010 + 001
const byte RECIPE_UNDONE_TONIC = 0x5; // 100 + 001

// { pin, dispense duration, recipe bitmask, time_left_ms )
fluid fluids[] = {
  {11, 2000, 0x2, 0}, // Gin
  {12, 3000, 0x4, 0}, // Undone
  {13, 5000, 0x1, 0}, // Tonic
};

const int INTERVAL = 100; // process every x ms

// variables
enum_state state;
unsigned long time_start, time_current;
int time_elapsed;
unsigned long last_step;
bool all_zero;
byte recipe_mask;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_SWITCH, INPUT_PULLUP);

  for(fluid fl : fluids) {
    pinMode(fl.pin, OUTPUT);
  }

  state = READY;
}

void loop() {
  switch (state) {
  case READY:
    if (digitalRead(PIN_BUTTON) == HIGH) return;

    // Button was pressed and we are ready

    // Read switch and set recipe
    recipe_mask = digitalRead(PIN_SWITCH) == HIGH ? RECIPE_GIN_TONIC : RECIPE_UNDONE_TONIC;

    for (fluid &fl : fluids) {
      // only add pour time if ingredient is in recipe
      if ( (recipe_mask & fl.fluid_bit) > 0) {
        fl.time_left_ms = fl.time_pour;
      }
    }
    
    time_start = millis();
    state = SERVING;
    break;
  case SERVING:
    time_current = millis();
    time_elapsed = time_current - time_start;
    time_start = time_current;

    all_zero = true;
    for (fluid &fl : fluids) {
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
