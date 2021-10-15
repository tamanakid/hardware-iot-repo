#ifndef _SKETCH_H
#define _SKETCH_H

// Library dependencies

#include <Arduino.h>
#include "scheduler.h"


// Constants
#define SECONDS_GREEN_LED_BLINK   10
#define SECONDS_ALARM_ON          20
#define SECONDS_ALARM_SLEEP       30


// Type definitions

enum e_state_action {
  ACTION_BUZZER_ACTIVATE,
  ACTION_BUZZER_DEACTIVATE,
};

typedef e_state_action t_state_action;

/*
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
*/


// Function declarations
void setSchedulerState(t_state_action action);

void setupWifiConnect(void);
void setupBuzzer(void);

void taskWifiConnect(void);
void taskBuzzer(void);

void resetWifiConnect(void);
void resetBuzzer(void);

/* Global reference variables */

extern schedulerTask *task_wifi_connect, *task_buzzer;

// extern t_global_state state;


#endif // _SKETCH_H
