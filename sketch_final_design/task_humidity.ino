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

  Serial.println("task:temperature> Reading humidity...");
  
  int humidity_read = Sht.getHumidity();
  
  state.humidity = humidity_read;
  humidityMeanRecalculate(state.humidity);

  Serial.print("task:humidity> Humidity read: ");
  Serial.println(state.humidity);
}


void resetHumidity () {
  
}


void humidityMeanRecalculate (int last_value) {
  state.humidity_mean_count++;

  state.humidity_mean_current = ((state.humidity_mean_count - 1)*state.humidity_mean_current + last_value) / state.humidity_mean_count;
}


void humidityMeanReadAndReset () {
  state.humidity_means[2] = state.humidity_means[1];
  state.humidity_means[1] = state.humidity_means[0];
  state.humidity_means[0] = state.humidity_mean_current;

  Serial.print("task:humidity> New humidity mean: ");
  Serial.println(state.humidity_mean_current);
    
  state.humidity_mean_count = 0;
  state.humidity_mean_current = 0;
}
