#include "hardware.h"

#include "sketch.h"


extern t_global_state state;


/* Task: Green LED Blink */

int green_rgb = 0;

void setupGreenLEDBlink() {
  pinMode(PIN_INT_LED_GREEN, OUTPUT);
}


void taskGreenLEDBlink() {  
  green_rgb = green_rgb == 0 ? 1 : 0;
  
  digitalWrite(PIN_INT_LED_GREEN, green_rgb);
}


void onResetGreenLEDBlink() {
  green_rgb = 0;
  digitalWrite(PIN_INT_LED_GREEN, green_rgb);
}
