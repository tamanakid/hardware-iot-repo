#include <stdlib.h>
#include <SHT21.h>

#include "hardware_d1mini.h"
#include "file_handler.h"
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
  
  int humidity_read = Sht.getHumidity() * 100/118;
  
  state.humidity.current.value = humidity_read;
  humidityMeanRecalculate(state.humidity.current.value);

  // Serial.print("task:humidity> Humidity read: ");
  // Serial.println(state.humidity.current.value);

  sprintf(state.humidity.current.timestamp, "%02d:%02d:%02d - %02d/%02d/%04d", state.time_clock.tm_hour, state.time_clock.tm_min, state.time_clock.tm_sec, state.time_clock.tm_mday, state.time_clock.tm_mon, state.time_clock.tm_year);

  // Serial.print("task:humidity> Humidity read at date ");
  // Serial.println(state.humidity.current.timestamp);

  state.humidity.is_alarm = (state.humidity.current.value > state.humidity.threshold) ? true : false;
  // Serial.print("task:humidity> Humidity alarm is ");
  // Serial.println(state.humidity.is_alarm ? "ON" : "OFF");
  
  String dat_string = "H (%): " + ((String) state.humidity.current.value);
  fileWriteWithTimestamp(state.current_files.file_dat, dat_string, &state.time_clock);

  if (state.humidity.is_alarm) {
    String log_string = "ALARM (Humidity) - Value: " + ((String) state.humidity.current.value) + " - Threshold: " + ((String) state.humidity.threshold);
    fileWriteWithTimestamp(state.current_files.file_log, log_string, &state.time_clock);
  }
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

  // Serial.print("task:humidity> New humidity mean: ");
  // Serial.println(state.humidity.mean_current);

  String mdat_string = "Humidity Mean (%): " + ((String) state.humidity.mean_current) + " (over " + ((String) state.humidity.mean_count) + " measurements)";
  fileWriteWithTimestamp(state.current_files.file_mdat, mdat_string, &state.time_clock);
    
  state.humidity.mean_count = 0;
  state.humidity.mean_current = 0;
}
