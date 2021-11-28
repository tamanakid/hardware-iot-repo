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

  Serial.println("task:temperature> Reading temperature...");
  
  float temperature_read = Sht.getTemperature();
  
  state.temperature = ((float)((int)(temperature_read * 10))) / 10;
  temperatureMeanRecalculate(state.temperature);

  Serial.print("task:temperature> Temperature read: ");
  Serial.println(state.temperature);
}


void resetTemperature() {
  
}


void temperatureMeanRecalculate (float last_value) {
  state.temperature_mean_count++;

  state.temperature_mean_current = ((state.temperature_mean_count - 1)*state.temperature_mean_current + last_value) / state.temperature_mean_count;
}


void temperatureMeanReadAndReset () {
  state.temperature_means[2] = state.temperature_means[1];
  state.temperature_means[1] = state.temperature_means[0];
  state.temperature_means[0] = state.temperature_mean_current;

  Serial.print("task:temperature> New temperature mean: ");
  Serial.println(state.temperature_mean_current);
  
  
  state.temperature_mean_count = 0;
  state.temperature_mean_current = 0;
}
