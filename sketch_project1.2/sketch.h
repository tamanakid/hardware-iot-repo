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

/*
enum e_scheduler_state {
  STATE_WRIST_SENSOR,
  STATE_NORMAL,
  STATE_GREEN_LED,
  STATE_ALARM,
  STATE_SLEEP
};
typedef e_scheduler_state t_scheduler_state;
*/

enum e_reducer_action {
  RDX_WRIST_ACTIVATE,
  RDX_WRIST_DEACTIVATE,
  RDX_GREEN_LED_ON,
  RDX_ALARM_ON,
  RDX_ALARM_SLEEP,
  RDX_ALARM_RESET, // ALARM_STANDBY
};

typedef e_reducer_action t_reducer_action;

enum e_alarm_state {
  ALARM_OFF, // ALARM_STANDBY
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
void setSchedulerState(t_reducer_action action);

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
    
    
/* Task reference variables */
extern schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler, *task_wrist_sensor;

extern t_global_state state;


#endif // _SKETCH_H
