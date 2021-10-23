#include <stdlib.h>
#include "hardware_d1mini.h"

#include "sketch.h"


void setupLEDTest() {
  pinMode(MINID1_PIN_D0, OUTPUT);
}

int led = 0;

void taskLEDTest() {
  led = (led == 0) ? 1 : 0;
  digitalWrite(MINID1_PIN_D0, led);
}


void resetLEDTest() {
  led = 0;
  digitalWrite(MINID1_PIN_D0, led);
}
