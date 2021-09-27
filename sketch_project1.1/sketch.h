#ifndef _SKETCH_H
#define _SKETCH_H

#include "scheduler.h"
#include <Arduino.h>


#ifdef __cplusplus
  extern "C" {    
    /* Function declarations */
    void taskLightSensor(void);
    void taskGreenLEDBlink(void);
    void taskAlarmHandler(void);
    void taskTimersTick(void);
  }
#endif
    
    
/* Task reference variables */
extern schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler;

/* State variables */
typedef struct {
  bool is_overlit;
  bool is_alarm;
  bool is_sleep;
  float seconds_overlit;
  float seconds_sleep;
} t_state;

extern t_state state;


#endif // _SKETCH_H


/**
 * Initially thought of a state enum type, but discarded since some states are independent
 * (i.e. GREEN_LED can toggle both in "NORMAL" and "ALARM" states(
 *
#define STATE_NORMAL        0
#define STATE_GREEN_LED     1
#define STATE_ALARM         2
#define STATE_SLEEP         3

typedef enum {
  STATE_NORMAL,
  STATE_GREEN_LED,
  STATE_ALARM,
  STATE_SLEEP
} t_state;
t_state state = STATE_NORMAL;
 */
