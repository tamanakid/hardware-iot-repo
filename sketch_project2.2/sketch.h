#ifndef _SKETCH_H
#define _SKETCH_H

// Library dependencies

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

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
  int hours;
  int minutes;
  int seconds;
} t_time;

typedef struct {
  int light;
  int light_threshold;
  bool is_overlit;
  bool is_server_active;
  t_time timestamp;
} t_global_state;



// Function declarations
void setSchedulerState(t_state_action action);

void setupWifiConnect(void);
void setupBuzzer(void);
void setupLightSensor(void);
void setupLEDTest(void);
void setupWebServer(void);
void setupClock(void);

void taskWifiConnect(void);
void taskBuzzer(void);
void taskLightSensor(void);
void taskLEDTest(void);
void taskWebServer(void);
void taskClock(void);

void resetWifiConnect(void);
void resetBuzzer(void);
void resetLightSensor(void);
void resetLEDTest(void);
void resetWebServer(void);
void resetClock(void);

/* Global reference variables */

extern schedulerTask *task_wifi_connect, *task_buzzer, *task_light_sensor, *task_led_test, *task_web_server, *task_clock;

extern ESP8266WebServer server;
extern WiFiUDP UdpInstance;
extern t_global_state state;


#endif // _SKETCH_H
