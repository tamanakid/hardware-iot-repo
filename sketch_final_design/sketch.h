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

enum e_scheduler_action {
  ACTION_ON_CONNECTED,
  ACTION_ON_DISCONNECTED,
};

typedef e_scheduler_action t_scheduler_action;


typedef struct {
  int hours;
  int minutes;
  int seconds;
  int epoch;
} t_time;

typedef struct {
  int values[12];
  int count;
} t_measures;

typedef struct {
  bool is_server_active;
  float temperature;
  int humidity;
  float temperature_means[3];
  float humidity_means[3];
  bool is_temperature_alarm;
  bool is_humidity_alarm;
  t_time timestamp;
  // t_measures measures;
} t_global_state;


// Function declarations
void updateScheduler(t_scheduler_action action);

void setupWifiConnect(void);
void setupWebServer(void);
void setupMeasurements(void);
void setupClock(void);

void taskWifiConnect(void);
void taskWebServer(void);
void taskMeasurements(void);
void taskClock(void);

void resetWifiConnect(void);
void resetWebServer(void);
void resetMeasurements(void);
void resetClock(void);

/* Global reference variables */

extern schedulerTask *task_wifi_connect, *task_web_server, *task_measurements, *task_clock;

extern ESP8266WebServer server;
extern WiFiUDP UdpInstance;
extern t_global_state state;


#endif // _SKETCH_H
