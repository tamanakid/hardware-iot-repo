#include <stdlib.h>
#include <SHT21.h>

#include "hardware_d1mini.h"
#include "sketch.h"


extern SHT21 Sht;
extern t_global_state state;

void setupTemperature () {
  Wire.begin();
}


void taskTemperature() {
  static bool initial_write = false;
  if (!initial_write) {
    // _flashInitFile();
    initial_write = true;
  }

  // Serial.println("task:temperature> Reading temperature...");
  
  float temperature_read = Sht.getTemperature();
  
  state.temperature.current.value = ((float)((int)(temperature_read * 10))) / 10;
  temperatureMeanRecalculate(state.temperature.current.value);

  Serial.print("task:temperature> Temperature read: ");
  Serial.println(state.temperature.current.value);
}


void resetTemperature() {
  
}


void temperatureMeanRecalculate (float last_value) {
  state.temperature.mean_count++;

  state.temperature.mean_current = ((state.temperature.mean_count - 1)*state.temperature.mean_current + last_value) / state.temperature.mean_count;
}


void temperatureMeanReadAndReset () {
  state.temperature.means[2] = state.temperature.means[1];
  state.temperature.means[1] = state.temperature.means[0];
  state.temperature.means[0] = state.temperature.mean_current;

  Serial.print("task:temperature> New temperature mean: ");
  Serial.println(state.temperature.mean_current);
  
  
  state.temperature.mean_count = 0;
  state.temperature.mean_current = 0;
}
