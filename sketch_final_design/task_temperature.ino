#include <stdlib.h>
#include <SHT21.h>
#include <cstring>

#include "FS.h"
#include "file_handler.h"

#include "hardware_d1mini.h"
#include "sketch.h"


extern SHT21 Sht;
extern t_global_state state;

void setupTemperature () {
  Wire.begin();

  temperatureRecordsSetup();
}


void taskTemperature() {
  static bool initial_write = false;
  if (!initial_write) {
    // _flashInitFile();
    initial_write = true;
  }

  Serial.println("task:temperature> Reading temperature...");
  
  float temperature_read = Sht.getTemperature();
  
  state.temperature.current.value = ((float)((int)(temperature_read * 10))) / 10;
  temperatureMeanRecalculate(state.temperature.current.value);

  Serial.print("task:temperature> Temperature read: ");
  Serial.println(state.temperature.current.value);
  
  sprintf(state.temperature.current.timestamp, "%02d:%02d:%02d - %02d/%02d/%04d", state.time_clock.tm_hour, state.time_clock.tm_min, state.time_clock.tm_sec, state.time_clock.tm_mday, state.time_clock.tm_mon, state.time_clock.tm_year);

  // Serial.print("task:temperature> Temperature read at date ");
  // Serial.println(state.temperature.current.timestamp);
  
  state.temperature.is_alarm = (state.temperature.current.value > state.temperature.threshold) ? true : false;
  // Serial.print("task:temperature> Temperature alarm is ");
  // Serial.println(state.temperature.is_alarm ? "ON" : "OFF");

  String dat_string = "T(ºC): " + ((String) state.temperature.current.value);
  fileWriteWithTimestamp(state.current_files.file_dat, dat_string, &state.time_clock);

  if (state.temperature.is_alarm) {
    String log_string = "ALARM (Temper'e) - Value: " + ((String) state.temperature.current.value) + " - Threshold: " + ((String) state.temperature.threshold);
    fileWriteWithTimestamp(state.current_files.file_log, log_string, &state.time_clock);
  }

  temperatureRecordsSync();
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

  // Serial.print("task:temperature> New temperature mean: ");
  // Serial.println(state.temperature.mean_current);

  String mdat_string = "Temper'e Mean(ºC): " + ((String) state.temperature.mean_current) + " (over " + ((String) state.temperature.mean_count) + " measurements)";
  fileWriteWithTimestamp(state.current_files.file_mdat, mdat_string, &state.time_clock);
  
  state.temperature.mean_count = 0;
  state.temperature.mean_current = 0;
}



void temperatureRecordsSetup() {
  if (SPIFFS.exists(state.current_files.file_temp_min)) {
    char min_contents[50];
    sprintf(min_contents, fileRead(state.current_files.file_temp_min).c_str());
    
    char *values = strtok(min_contents, "measured");
    state.temperature.record_min.value = atof(values);

    Serial.print("Min temperature value read from file: ");
    Serial.println(state.temperature.record_min.value);

    values = strtok (NULL, "measured");
    Serial.print("timestamp: ");
    sprintf(state.temperature.record_min.timestamp, values);
    Serial.println(state.temperature.record_min.timestamp);
  }
  
  if (SPIFFS.exists(state.current_files.file_temp_max)) {
    char max_contents[50];
    sprintf(max_contents, fileRead(state.current_files.file_temp_max).c_str());
    
    char *values = strtok(max_contents, "measured");
    state.temperature.record_max.value = atof(values);
    
    Serial.print("Max temperature value read from file: ");
    Serial.println(state.temperature.record_max.value);

    values = strtok (NULL, "measured");
    Serial.print("timestamp: ");
    sprintf(state.temperature.record_max.timestamp, values);
    Serial.println(state.temperature.record_max.timestamp);
  }
}


void temperatureRecordsSync() {
  bool file_min_exists = SPIFFS.exists(state.current_files.file_temp_min);
  bool file_max_exists = SPIFFS.exists(state.current_files.file_temp_max);
  
  if (!file_min_exists || state.temperature.record_min.value == NULL || (state.temperature.current.value < state.temperature.record_min.value)) {
    state.temperature.record_min.value = state.temperature.current.value;
    sprintf(state.temperature.record_min.timestamp, state.temperature.current.timestamp);

    Serial.print("New min temperature value: ");
    Serial.println(state.temperature.record_min.value);
    
    String file_contents = (String)state.temperature.record_min.value + " measured " + state.temperature.record_min.timestamp;
    fileOverwrite(state.current_files.file_temp_min, file_contents);
    
    Serial.println("Content written to file");
  }

  if (!file_max_exists || state.temperature.record_max.value == NULL || (state.temperature.current.value > state.temperature.record_max.value)) {
    state.temperature.record_max.value = state.temperature.current.value;
    sprintf(state.temperature.record_max.timestamp, state.temperature.current.timestamp);

    Serial.print("New max temperature value: ");
    Serial.println(state.temperature.record_max.value);
    
    String file_contents = (String)state.temperature.record_max.value + " measured " + state.temperature.record_max.timestamp;
    fileOverwrite(state.current_files.file_temp_max, file_contents);

    Serial.println("Content written to file");
  }
}
