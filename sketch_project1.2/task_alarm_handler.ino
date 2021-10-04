#include <Arduino.h>

#include "hardware.h"
#include "scheduler.h"

#include "sketch.h"


extern t_global_state state;


/* Task: Handle Alarm */

bool is_tone_set = false;

void setupAlarmHandler () {
  pinMode(PIN_EXT_BUZZER, OUTPUT);
  pinMode(PIN_EXT_BUTTON, INPUT_PULLUP);
}


void taskAlarmHandler() {  
  if (!is_tone_set) {
    is_tone_set = true;
    tone(PIN_EXT_BUZZER, BUZZER_NOTE_A5);
  }
  
  int buttonState = digitalRead(PIN_EXT_BUTTON);

  if (buttonState == LOW) {    
    setSchedulerState(RDX_ALARM_SLEEP);
  }
}


void onResetAlarmHandler() {
  noTone(PIN_EXT_BUZZER);
  is_tone_set = false;
}
