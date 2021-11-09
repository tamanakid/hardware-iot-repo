#include <stdlib.h>
#include "FS.h"

#include "hardware_d1mini.h"
#include "sketch.h"

#define PATH_LIGHT_MEASURES "/light-measures.txt"



extern t_global_state state;

File light_measures_file;


void setupLightSensor () {
  pinMode(MINID1_PIN_A0, INPUT);
}


void taskLightSensor() {
  static bool initial_write = false;
  if (!initial_write) {
    _flashInitFile();
    initial_write = true;
  }
  
  int analog_read = analogRead(MINID1_PIN_A0);
  // Serial.print("Light read: ");
  // Serial.println(analog_read);

  state.light = (analog_read * 3.0157) - 8.094;

  if (state.light > state.light_threshold) {
    state.is_overlit = true;
  } else {
    state.is_overlit = false;
    if (state.is_server_active) {
      digitalWrite(MINID1_PIN_D0, LOW);
      state.is_alarm_active = false;
    }
  }

  _saveMeasure();
}


void _saveMeasure() {
  // Write into measures file
  if (state.is_server_active) {
    
    char text_measure[30];
    sprintf(
      text_measure,
      "%02d:%02d:%02d - %04d mV - %s\n",
      state.timestamp.hours,
      state.timestamp.minutes,
      state.timestamp.seconds,
      state.light,
      state.is_overlit ? "overlit!" : "underlit"
    );

    Serial.print(state.light);
    Serial.print("...");
    // Serial.print("task:LightSensor> Light measurement:");
    // Serial.print(">>>");
    // Serial.print(text_measure);

    state.measures.values[state.measures.count] = state.light;
    state.measures.count++;

    if (state.measures.count == 12) {
      state.measures.count = 0;

      // Calculate mean
      int mean = 0;
      for (int i = 0; i < 12; i++) {
        mean += state.measures.values[i];
      }
      mean = mean / 12;

      bool mean_is_overlit = (mean > state.light_threshold);

      // Format text
      char text_mean[31];
      sprintf(
        text_mean,
        "%02d:%02d:%02d - %04d mV - %s\n",
        state.timestamp.hours,
        state.timestamp.minutes,
        state.timestamp.seconds,
        mean,
        mean_is_overlit ? "overlit!" : "underlit"
      );

      size_t bytes = light_measures_file.print(text_mean);
      if (bytes <= 0) {
        Serial.println("task:LightSensor> Error writing measure to measures file");
      } else {
        Serial.println("");
        Serial.print("File Size: ");
        Serial.println(light_measures_file.size());
        // Serial.println("");
        Serial.println("task:LightSensor> Mean written to measures file:");
        Serial.print(">>>");
        Serial.print(text_mean);
        Serial.println("");
      }
    }

  } else {
    Serial.println("task:LightSensor> Measure not saved to measures file since server was detected as inactive");
  }

  /* Debugging */
  // char *buffer = (char*) malloc(40*sizeof(char));
  // sprintf(buffer, "light: %d", light);
  // Serial.println(buffer);
}


void _flashInitFile() {
  if (!SPIFFS.begin()) {
    Serial.println("task:LightSensor> Error intializing SPIFFS");
    return;
  }

  File light_measures_content = SPIFFS.open(PATH_LIGHT_MEASURES, "r");
  if (light_measures_content) {    
    Serial.println("task:LightSensor> Previous light measures:");
    Serial.print("task:LightSensor> File Size: ");
    Serial.println(light_measures_content.size());
    
    while(light_measures_content.available()) {
      Serial.write(light_measures_content.read()); 
    }
    
  } else {
    Serial.println("task:LightSensor> No light measures content");
  }
  light_measures_content.close();
  
  light_measures_file = SPIFFS.open(PATH_LIGHT_MEASURES, "w");
  if (!light_measures_file) {
    Serial.println("task:LightSensor> Error opening light measures file");
    return;
  }
}


void resetLightSensor() {
  
}
