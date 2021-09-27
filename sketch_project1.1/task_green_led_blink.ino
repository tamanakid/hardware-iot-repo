#include "hardware.h"

#include "sketch.h"


extern t_state state;


/* Task: Green LED Blink */

// int green_rgb = 0;

void taskGreenLEDBlink() {
  static int green_rgb = 0;
  
  if (state.seconds_overlit >= 5) {
    green_rgb = green_rgb == 0 ? 1 : 0;
  } else {
    green_rgb = 0;
  }
  
  digitalWrite(PIN_INT_LED_GREEN, green_rgb);
}
