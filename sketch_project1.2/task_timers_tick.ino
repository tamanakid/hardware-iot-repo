#include "scheduler.h"

#include "sketch.h"


extern t_global_state state;


void setupTimersTick() {
  
}

void taskTimersTick () {
  
  if (state.alarm_state == ALARM_SLEEP) {
    state.seconds_sleep += 0.1;
    state.seconds_overlit = 0;
    if (state.seconds_sleep >= SECONDS_ALARM_SLEEP) {
      setSchedulerState(ACTION_ALARM_STANDBY);
    }
    
  } else if (state.alarm_state == ALARM_STANDBY) {
    state.seconds_overlit = (state.is_overlit) ? state.seconds_overlit + 0.1 : 0;
    state.seconds_sleep = 0;

    if (state.seconds_overlit >= SECONDS_ALARM_ON) {
      setSchedulerState(ACTION_ALARM_ON);
    } else if (state.seconds_overlit >= SECONDS_GREEN_LED_BLINK) {
      setSchedulerState(ACTION_GREEN_LED_ON);
    } else if (!state.is_overlit) {
      setSchedulerState(ACTION_ALARM_STANDBY);
    }
  }
}
