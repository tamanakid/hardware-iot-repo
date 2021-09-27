#include "scheduler.h"

#include "sketch.h"


extern t_state state;
extern schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler;


void taskTimersTick () {
  
  if (state.is_sleep) {
    state.seconds_sleep += 0.1;
    state.seconds_overlit = 0;
    if (state.seconds_sleep >= 15) {
      state.is_sleep = false;
      scheduler_activate(task_light_sensor);
    }
    
  } else if (!state.is_alarm) {
    state.seconds_overlit = (state.is_overlit) ? state.seconds_overlit + 0.1 : 0;
    state.seconds_sleep = 0;

    if (state.seconds_overlit >= 10) {
      scheduler_activate(task_alarm_handler);
    } else if (state.seconds_overlit >= 5) {
      scheduler_activate(task_green_led_blink);
    } else if (state.is_overlit) {
      scheduler_deactivate(task_alarm_handler);
      scheduler_deactivate(task_green_led_blink);
    }
  }
}
