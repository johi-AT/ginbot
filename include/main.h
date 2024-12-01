#include <Arduino.h>

enum stateEnum { READY, SERVING };

typedef struct {
  const byte pin;
  const int time_pour;
  int time_left_ms;
} fluid;
