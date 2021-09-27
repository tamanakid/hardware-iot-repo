#include <Arduino.h>

#include "hardware.h"
#include "scheduler.h"

#include "sketch.h"


extern t_state state;


/* Task: Handle Alarm */

void taskAlarmHandler() {
  if (!state.is_alarm) {
    state.is_alarm = true;
    tone(PIN_EXT_BUZZER, BUZZER_NOTE_A5);
  }
  
  int buttonState = digitalRead(PIN_EXT_BUTTON);

  if (state.is_alarm && buttonState == LOW) {
    state.is_alarm = false;
    state.is_sleep = true;
    noTone(PIN_EXT_BUZZER);
    
    scheduler_deactivate(task_light_sensor);
    scheduler_deactivate(task_green_led_blink);
    scheduler_deactivate(task_alarm_handler);
  }
}
