#include <stdlib.h>
#include "hardware_d1mini.h"

#include "sketch.h"


extern t_global_state state;


void setupLightSensor () {
  pinMode(MINID1_PIN_A0, INPUT);
}


void taskLightSensor() {  
  state.light = analogRead(MINID1_PIN_A0);

  if (state.light > state.light_threshold) {
    state.is_overlit = true;
  } else {
    state.is_overlit = false;
    if (state.is_server_active) {
      digitalWrite(MINID1_PIN_D0, LOW);
      state.is_alarm_active = false;
    }
  }

  /* Debugging */
  // char *buffer = (char*) malloc(40*sizeof(char));
  // sprintf(buffer, "light: %d", light);
  // Serial.println(buffer);
}


void resetLightSensor() {
  
}
