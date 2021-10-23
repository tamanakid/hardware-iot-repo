#include <stdlib.h>
#include "hardware_d1mini.h"

#include "sketch.h"



// TODO: Likely needs not be a task, but merely functionality within the web server

/* Extern variables */

extern t_global_state state;


/* Constants */

#define LIGHT_SENSOR_NOISE 20
#define LIGHT_SENSOR_RANGE (1020 - LIGHT_SENSOR_NOISE)
#define LIGHT_SENSOR_THRESHOLD 500


/* Task: Light sensor */

int light = 0;


void setupLightSensor () {
  pinMode(MINID1_PIN_A0, INPUT);
}


void taskLightSensor() {  
  light = analogRead(MINID1_PIN_A0);
  state.light = light;

  if (light > LIGHT_SENSOR_THRESHOLD) {
    state.is_overlit = true;
  } else {
    state.is_overlit = false;
  }

  /* Debugging */
  // char *buffer = (char*) malloc(40*sizeof(char));
  // sprintf(buffer, "light: %d", light);
  // Serial.println(buffer);
}


void resetLightSensor() {
  
}
