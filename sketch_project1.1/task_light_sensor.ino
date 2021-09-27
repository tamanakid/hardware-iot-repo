#include <stdlib.h>
#include "hardware.h"

#include "sketch.h"


/* Extern variables */

extern t_state state;


/* Constants */

#define LIGHT_SENSOR_NOISE 20
#define LIGHT_SENSOR_RANGE (1020 - LIGHT_SENSOR_NOISE)
#define LIGHT_SENSOR_STEPS 6
#define LIGHT_SENSOR_THRESHOLD_STEP 3


/* Type definitions */

typedef struct {
  float step_size;
  int pin_led_bar;
} t_light_step;


/* Local variables */

t_light_step light_steps[LIGHT_SENSOR_STEPS] = {
  {0.5, PIN_INT_LED_BAR_0},
  {1.5, PIN_INT_LED_BAR_1},
  {2.5, PIN_INT_LED_BAR_2},
  {3.5, PIN_INT_LED_BAR_3},
  {4.5, PIN_INT_LED_BAR_4},
  {5.5, PIN_INT_LED_BAR_5}
};


/* Task: Light sensor */

// int light = 0;

void taskLightSensor() {
  static int light = 0;
  
  light = analogRead(PIN_EXT_LIGHT_SENSOR);

  /* Debugging */
  char *buffer = (char*) malloc(40*sizeof(char));
  // sprintf(buffer, "light: %d", light);
  // Serial.println(buffer);

  /* Set the LED bars */
  for (int i = 0; i < LIGHT_SENSOR_STEPS; i++) {
    t_light_step stp = light_steps[i];
    int is_step_active = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*stp.step_size + LIGHT_SENSOR_NOISE) ? 1 : 0;
    
    digitalWrite(stp.pin_led_bar, is_step_active);

    /* Check light threshold: whether sensor is overlit */
    if (LIGHT_SENSOR_THRESHOLD_STEP == i) {
      state.is_overlit = (is_step_active) ? true : false;
    }
  }
}
