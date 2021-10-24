#ifndef _SKETCH_H
#define _SKETCH_H

// Library dependencies

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "scheduler.h"


// Constants
#define SECONDS_GREEN_LED_BLINK   10
#define SECONDS_ALARM_ON          20
#define SECONDS_ALARM_SLEEP       30


// Type definitions

enum e_state_action {
  ACTION_ON_CONNECTED,
  ACTION_ON_DISCONNECTED,
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

*/

typedef struct {
  bool is_overlit;
  bool is_server_active;
  int light;
} t_global_state;



// Function declarations
void setSchedulerState(t_state_action action);

void setupWifiConnect(void);
void setupBuzzer(void);
void setupLightSensor(void);
void setupLEDTest(void);
void setupWebServer(void);

void taskWifiConnect(void);
void taskBuzzer(void);
void taskLightSensor(void);
void taskLEDTest(void);
void taskWebServer(void);

void resetWifiConnect(void);
void resetBuzzer(void);
void resetLightSensor(void);
void resetLEDTest(void);
void resetWebServer(void);

/* Global reference variables */

extern schedulerTask *task_wifi_connect, *task_buzzer, *task_light_sensor, *task_led_test, *task_web_server;

extern ESP8266WebServer server;
extern t_global_state state;


#endif // _SKETCH_H
