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
    initLightMeasuresFile();
    initial_write = true;
  }
  
  state.light = analogRead(MINID1_PIN_A0);
  Serial.print("Light read: ");
  Serial.println(state.light);

  if (state.light > state.light_threshold) {
    state.is_overlit = true;
  } else {
    state.is_overlit = false;
    if (state.is_server_active) {
      digitalWrite(MINID1_PIN_D0, LOW);
      state.is_alarm_active = false;
    }
  }

  // Write into measures file
  if (state.is_server_active) {
    char content[28];

    sprintf(
      content,
      "%02d:%02d:%02d - %03d - %s\n",
      state.timestamp.hours,
      state.timestamp.minutes,
      state.timestamp.seconds,
      state.light * 100/1024,
      state.is_overlit ? "overlit!" : "underlit"
    );

    size_t bytes = light_measures_file.write(content, 28);

    if (bytes <= 0) {
      Serial.println("task:LightSensor> Error writing measure to measures file");
    } else {
      // Serial.println(light_measures_file.size());
      Serial.println("task:LightSensor> Measure written to measures file:");
      Serial.print(">>>");
      Serial.print(content);
    }
  } else {
    Serial.println("task:LightSensor> Measure not saved to measures file since server was detected as inactive");
  }

  /* Debugging */
  // char *buffer = (char*) malloc(40*sizeof(char));
  // sprintf(buffer, "light: %d", light);
  // Serial.println(buffer);
}

void initLightMeasuresFile() {
  if (!SPIFFS.begin()) {
    Serial.println("task:LightSensor> Error intializing SPIFFS");
    return;
  }

  // if (!exists(PATH_LIGHT_MEASURES)) {
  File light_measures_content = SPIFFS.open(PATH_LIGHT_MEASURES, "r");
  if (light_measures_content) {
    // Serial.println(light_measures_content.size());
    
    Serial.println("task:LightSensor> Previous light measures:");
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
