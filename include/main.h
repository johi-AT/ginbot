#include <Arduino.h>

enum enum_state { READY, SERVING };

typedef struct {
  const byte pin;
  const int time_pour;
  const byte fluid_bit;
  int time_left_ms;
} fluid;

void handle_pressure();
