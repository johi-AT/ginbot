#include <main.h>
#include <Arduino.h>

const int buttonPin = 16;

const fluid DEFAULT_FLUID_GIN = {25, 2000};
const fluid DEFAULT_FLUID_UNDONE = {13, 3000};
const fluid DEFAULT_FLUID_TONIC = {12, 5000};

const int INTERVAL = 100; // process every x ms

fluid fluid_gin, fluid_undone, fluid_tonic;

// variables
stateEnum state;
unsigned long startTime, currentTime;
int elapsedTime;

unsigned long last_step;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(fluid_gin.pin, OUTPUT);
  pinMode(fluid_undone.pin, OUTPUT);
  pinMode(fluid_tonic.pin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  state = READY;
}

void loop() {
  switch (state) {
  case READY:
    if (digitalRead(buttonPin) == HIGH) return;

    // Button was pressed and we are ready

    fluid_gin = DEFAULT_FLUID_GIN;
    fluid_undone = DEFAULT_FLUID_UNDONE;
    fluid_tonic = DEFAULT_FLUID_TONIC;

    startTime = millis();
    state = SERVING;
    break;
  case SERVING:
    currentTime = millis();
    elapsedTime = currentTime - startTime;
    startTime = currentTime;

    fluid_gin.time_left_ms = max(0, fluid_gin.time_left_ms - elapsedTime);
    fluid_undone.time_left_ms = max(0, fluid_undone.time_left_ms - elapsedTime);
    fluid_tonic.time_left_ms = max(0, fluid_tonic.time_left_ms - elapsedTime);

    if ( currentTime - last_step >= INTERVAL ) {
      Serial.print("time_left_ms: GIN: ");
      Serial.print(fluid_gin.time_left_ms);
      Serial.print(" UNDONE: ");
      Serial.print(fluid_undone.time_left_ms);
      Serial.print(" TONIC: ");
      Serial.println(fluid_tonic.time_left_ms);
      last_step = millis();
    }
    
    digitalWrite(fluid_gin.pin, fluid_gin.time_left_ms > 0 ? HIGH : LOW);
    digitalWrite(fluid_undone.pin, fluid_undone.time_left_ms > 0 ? HIGH : LOW);
    digitalWrite(fluid_tonic.pin, fluid_tonic.time_left_ms > 0 ? HIGH : LOW);

    if ( (fluid_gin.time_left_ms == 0) &&
         (fluid_undone.time_left_ms == 0) &&
         (fluid_tonic.time_left_ms == 0) ) {
      state = READY;
      Serial.println("READY");
    }
    break;
  }
}