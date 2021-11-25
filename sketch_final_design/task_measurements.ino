#include <stdlib.h>

#include "hardware_d1mini.h"
#include "sketch.h"

#define PATH_LIGHT_MEASURES "/light-measures.txt"



extern t_global_state state;

File light_measures_file;


void setupMeasurements () {
  pinMode(MINID1_PIN_A0, INPUT);
}


void taskMeasurements() {
  static bool initial_write = false;
  if (!initial_write) {
    // _flashInitFile();
    initial_write = true;
  }
  
  int analog_read = analogRead(MINID1_PIN_A0);
  // Serial.print("Light read: ");
  // Serial.println(analog_read);

  state.temperature = analog_read * 0.03;
  state.humidity = analog_read / 12;
}


void resetMeasurements() {
  
}
