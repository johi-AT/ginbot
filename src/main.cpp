#include <main.h>
#include <Arduino.h>

const int buttonPin = 16;

const fluid DEFAULT_FLUID_GIN = {25, 3000};

fluid fluid_gin;

// variables
int buttonState = 0;
stateEnum state;
unsigned long startTime;
unsigned long elapsedTime;
bool all_times_zero;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonPin, INPUT);
  state = READY;
}

void loop()
{
  switch (state)
  {
  case READY:
    if (digitalRead(buttonPin) == LOW)
      return;

    // Button was pressed and we are ready

    // TODO: Assign default time array
    fluid_gin = DEFAULT_FLUID_GIN;
    startTime = millis();
    state = SERVING;
    break;
  case SERVING:
    elapsedTime = millis() - startTime;

    fluid_gin.time_left_ms = max(0, fluid_gin.time_left_ms - elapsedTime);

    all_times_zero = true;

    if (fluid_gin.time_left_ms > 0)
    {
      digitalWrite(fluid_gin.pin, HIGH);
      all_times_zero = false;
    }
    else
    {
      digitalWrite(fluid_gin.pin, LOW);
    }

    if(all_times_zero){
      state = READY;
    }
    break;

  default:
    break;
  }

  // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  // delay(1000);               // wait for a second
  // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  // delay(1000);               // wait for a second
}
