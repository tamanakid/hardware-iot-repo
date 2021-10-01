#ifndef _SKETCH_H
#define _SKETCH_H

#include <Arduino.h>
#include "scheduler.h"

/*
#define STATE_NORMAL 0
#define STATE_GREEN_LED 1
#define STATE_ALARM 2
#define STATE_SLEEP 3


int STATE_NORMAL = 0;
int STATE_GREEN_LED = 1;
int STATE_ALARM = 2;
int STATE_SLEEP = 3;
*/

enum e_scheduler_state {
  STATE_NORMAL,
  STATE_GREEN_LED,
  STATE_ALARM,
  STATE_SLEEP
};

typedef e_scheduler_state t_scheduler_state;

typedef struct {
  bool is_overlit;
  t_scheduler_state scheduler_state;
  float seconds_overlit;
  float seconds_sleep;
} t_global_state;


/* Function declarations */
void setSchedulerState(t_scheduler_state new_state);

void taskLightSensor(void);
void taskGreenLEDBlink(void);
void taskAlarmHandler(void);
void taskTimersTick(void);

void onResetLightSensor(void);
void onResetGreenLEDBlink(void);
void onResetAlarmHandler(void);
    
    
/* Task reference variables */
extern schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler;

extern t_global_state state;


#endif // _SKETCH_H
