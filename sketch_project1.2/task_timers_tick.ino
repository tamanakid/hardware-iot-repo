#include "scheduler.h"

#include "sketch.h"


extern t_global_state state;


void setupTimersTick() {
  
}

void taskTimersTick () {
  
  if (state.alarm_state == ALARM_SLEEP) {
    state.seconds_sleep += 0.1;
    state.seconds_overlit = 0;
    if (state.seconds_sleep >= 15) {
      setSchedulerState(RDX_ALARM_RESET);
    }
    
  } else if (state.alarm_state == ALARM_OFF) {
    state.seconds_overlit = (state.is_overlit) ? state.seconds_overlit + 0.1 : 0;
    state.seconds_sleep = 0;

    if (state.seconds_overlit >= 10) {
      setSchedulerState(RDX_ALARM_ON);
    } else if (state.seconds_overlit >= 5) {
      setSchedulerState(RDX_GREEN_LED_ON);
    } else if (!state.is_overlit) {
      setSchedulerState(RDX_ALARM_RESET);
    }
  }
}
