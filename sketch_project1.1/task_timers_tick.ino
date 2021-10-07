#include "scheduler.h"

#include "sketch.h"


extern t_global_state state;
extern schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler;


void taskTimersTick () {
  
  if (state.scheduler_state == STATE_SLEEP) {
    state.seconds_sleep += 0.1;
    state.seconds_overlit = 0;
    if (state.seconds_sleep >= 30) {
      setSchedulerState(STATE_NORMAL);
    }
    
  } else if (state.scheduler_state != STATE_ALARM) {
    state.seconds_overlit = (state.is_overlit) ? state.seconds_overlit + 0.1 : 0;
    state.seconds_sleep = 0;

    if (state.seconds_overlit >= 20) {
      setSchedulerState(STATE_ALARM);
    } else if (state.seconds_overlit >= 10) {
      setSchedulerState(STATE_GREEN_LED);
    } else if (!state.is_overlit) {
      setSchedulerState(STATE_NORMAL);
    }
  }
}
