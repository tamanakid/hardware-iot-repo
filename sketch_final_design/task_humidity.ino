#include <stdlib.h>
#include <SHT21.h>

#include "hardware_d1mini.h"
#include "sketch.h"


extern SHT21 Sht;
extern t_global_state state;

void setupHumidity () {
  
}


void taskHumidity () {
  static bool initial_write = false;
  if (!initial_write) {
    // _flashInitFile();
    initial_write = true;
  }

  // Serial.println("task:temperature> Reading humidity...");
  
  int humidity_read = Sht.getHumidity();
  
  state.humidity.current.value = humidity_read;
  humidityMeanRecalculate(state.humidity.current.value);
  sprintf(state.humidity.current.timestamp, "%2d:%2d:%2d", state.time_clock.hour, state.time_clock.minute, state.time_clock.second);

  Serial.print("task:humidity> Humidity read: ");
  Serial.println(state.humidity.current.value);
}


void resetHumidity () {
  
}


void humidityMeanRecalculate (int last_value) {
  state.humidity.mean_count++;

  state.humidity.mean_current = ((state.humidity.mean_count - 1)*state.humidity.mean_current + last_value) / state.humidity.mean_count;
}


void humidityMeanReadAndReset () {
  state.humidity.means[2] = state.humidity.means[1];
  state.humidity.means[1] = state.humidity.means[0];
  state.humidity.means[0] = state.humidity.mean_current;

  Serial.print("task:humidity> New humidity mean: ");
  Serial.println(state.humidity.mean_current);
    
  state.humidity.mean_count = 0;
  state.humidity.mean_current = 0;
}
