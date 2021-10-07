#ifndef _SKETCH_H
#define _SKETCH_H

/* Library dependencies */

#include <Arduino.h>
#include "scheduler.h"


/* Constants */
#define SECONDS_GREEN_LED_BLINK   10
#define SECONDS_ALARM_ON          20
#define SECONDS_ALARM_SLEEP       30


/* Type definitions */

enum e_state_action {
  ACTION_WRIST_ACTIVATE,
  ACTION_WRIST_DEACTIVATE,
  ACTION_GREEN_LED_ON,
  ACTION_ALARM_ON,
  ACTION_ALARM_SLEEP,
  ACTION_ALARM_STANDBY,
};

typedef e_state_action t_state_action;


enum e_alarm_state {
  ALARM_STANDBY,
  ALARM_ON,
  ALARM_SLEEP,
};

typedef e_alarm_state t_alarm_state;


typedef struct {
  bool is_overlit;
  bool is_wrist_active;
  t_alarm_state alarm_state;
  float seconds_overlit;
  float seconds_sleep;
} t_global_state;


/* Function declarations */
void setSchedulerState(t_state_action action);

void setupLightSensor(void);
void setupGreenLEDBlink(void);
void setupAlarmHandler(void);
void setupTimersTick(void);
void setupWristSensor(void);

void taskLightSensor(void);
void taskGreenLEDBlink(void);
void taskAlarmHandler(void);
void taskTimersTick(void);
void taskWristSensor(void);

void onResetLightSensor(void);
void onResetGreenLEDBlink(void);
void onResetAlarmHandler(void);
    
    
/* Global reference variables */

extern schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler, *task_wrist_sensor;

extern t_global_state state;


#endif // _SKETCH_H
