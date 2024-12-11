#include <main.h>
#include <Arduino.h>

const byte PIN_BUTTON = 22;
const byte PIN_SWITCH = 27;
const byte PIN_COMPRESSOR = 2;
const byte PIN_ADC_PRESSURE = 26; // GPIO26 is ADC0

const int P_LOW = 400;  // get value by measuring
const int P_HIGH = 700; // get value by measuring

const byte RECIPE_GIN_TONIC = 0x3;    // 010 + 001
const byte RECIPE_UNDONE_TONIC = 0x5; // 100 + 001

// { pin, dispense duration, recipe bitmask, time_left_ms )
fluid fluids[] = {
  {19, 2000, 0x2, 0}, // Gin
  {20, 3000, 0x4, 0}, // Undone
  {21, 5000, 0x1, 0}, // Tonic
};

const int INTERVAL = 100; // process every x ms

// variables
enum_state state;
unsigned long time_start, time_current, last_step;
int time_elapsed;
bool all_zero;
byte recipe_mask;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_COMPRESSOR, OUTPUT);

  for(fluid fl : fluids) {
    pinMode(fl.pin, OUTPUT);
  }

  state = READY;
}

void loop() {
  time_current = millis();

  handle_pressure();

  switch (state) {
  case READY:
    if (digitalRead(PIN_BUTTON) == LOW) return;

    // Button was pressed and we are ready

    // Read switch and set recipe
    recipe_mask = digitalRead(PIN_SWITCH) == HIGH ? RECIPE_UNDONE_TONIC: RECIPE_GIN_TONIC;

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

void handle_pressure() {
  const float PRESSURE_WEIGHT = 0.1; // range [0,1]; higher = less smoothing
  static unsigned long p_last_step;
  static float pressure_prev;
  float pressure;
  unsigned long p_time_current = millis();
  bool compressor_on;

  // read pressure and control compressor every INTERVAL ms
  if ( p_time_current - p_last_step >= INTERVAL ) {
    // exponential smoothing
    pressure = analogRead(PIN_ADC_PRESSURE) * PRESSURE_WEIGHT + (1.0 - PRESSURE_WEIGHT) * pressure_prev;
    pressure_prev = pressure;
    p_last_step = millis();

    // compressor handling with hysteresis
    compressor_on = digitalRead(PIN_COMPRESSOR) == HIGH;

    if(compressor_on && (pressure >= P_HIGH) ) {
      digitalWrite(PIN_COMPRESSOR, LOW);
    } else if ( ! compressor_on && (pressure <= P_LOW) ) {
      digitalWrite(PIN_COMPRESSOR, HIGH);
    }
  }
}
