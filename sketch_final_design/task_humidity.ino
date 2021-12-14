#include <stdlib.h>
#include <SHT21.h>

#include "hardware_d1mini.h"
#include "file_handler.h"
#include "sketch.h"


extern SHT21 Sht;
extern t_global_state state;

void setupHumidity () {
  humidityRecordsSetup();
}


void taskHumidity () {
  static bool initial_write = false;
  if (!initial_write) {
    // _flashInitFile();
    initial_write = true;
  }

  // Serial.println("task:humidity> Reading humidity...");
  
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

  humidityRecordsSync();
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




void humidityRecordsSetup() {
  if (SPIFFS.exists(state.current_files.file_humi_min)) {
    char min_contents[50];
    sprintf(min_contents, fileRead(state.current_files.file_humi_min).c_str());
    
    char *values = strtok(min_contents, " measured at ");
    state.humidity.record_min.value = atof(values);
    
    Serial.print("Min humidity value read from file: ");
    Serial.println(state.humidity.record_min.value);

    values = strtok (NULL, " measured at ");
    Serial.print("timestamp: ");
    sprintf(state.humidity.record_min.timestamp, values);
    Serial.println(state.humidity.record_min.timestamp);
  }
  
  if (SPIFFS.exists(state.current_files.file_humi_max)) {
    char max_contents[50];
    sprintf(max_contents, fileRead(state.current_files.file_humi_max).c_str());
    
    char *values = strtok(max_contents, " measured at ");
    state.humidity.record_max.value = atof(values);

    Serial.print("Max humidity value read from file: ");
    Serial.println(state.humidity.record_max.value);

    values = strtok (NULL, " measured at ");
    Serial.print("timestamp: ");
    sprintf(state.humidity.record_max.timestamp, values);
    Serial.println(state.humidity.record_max.timestamp);
  }
}


void humidityRecordsSync() {
  bool file_min_exists = SPIFFS.exists(state.current_files.file_humi_min);
  bool file_max_exists = SPIFFS.exists(state.current_files.file_humi_max);
  
  if (!file_min_exists || state.humidity.record_min.value == NULL || (state.humidity.current.value < state.humidity.record_min.value)) {
    state.humidity.record_min.value = state.humidity.current.value;
    sprintf(state.humidity.record_min.timestamp, state.humidity.current.timestamp);

    Serial.print("New min humidity value: ");
    Serial.println(state.humidity.record_min.value);
    
    String file_contents = (String)state.humidity.record_min.value + " measured at " + state.humidity.record_min.timestamp;
    fileOverwrite(state.current_files.file_humi_min, file_contents);
    
    Serial.println("Content written to file");
  }

  if (!file_max_exists || state.humidity.record_max.value == NULL || (state.humidity.current.value > state.humidity.record_max.value)) {
    state.humidity.record_max.value = state.humidity.current.value;
    sprintf(state.humidity.record_max.timestamp, state.humidity.current.timestamp);

    Serial.print("New max humidity value: ");
    Serial.println(state.humidity.record_max.value);
    
    String file_contents = (String)state.humidity.record_max.value + " measured at " + state.humidity.record_max.timestamp;
    fileOverwrite(state.current_files.file_humi_max, file_contents);

    Serial.println("Content written to file");
  }
}
