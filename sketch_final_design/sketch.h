#ifndef _SKETCH_H
#define _SKETCH_H

// Library dependencies

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <SHT21.h>
#include "time.h"

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
  String file_dat;
  String file_mdat;
  String file_log;
} t_files;


typedef struct {
  float value;
  char timestamp[30];
} t_measure_temperature;

typedef struct {
  int value;
  char timestamp[30];
} t_measure_humidity;

typedef struct {
  t_measure_temperature current;
  float mean_current;
  int mean_count;
  float means[3];
  t_measure_temperature record_min;
  t_measure_temperature record_max;
  float threshold;
  bool is_alarm;
} t_temperature_info;

typedef struct {
  t_measure_humidity current;
  int mean_current;
  int mean_count;
  int means[3];
  t_measure_humidity record_min;
  t_measure_humidity record_max;
  int threshold;
  bool is_alarm;
} t_humidity_info;


typedef struct {
  t_temperature_info temperature;
  t_humidity_info humidity;
  bool is_server_active;
  struct tm time_clock;
  t_files current_files;
  bool is_ntp_updated;
  bool is_first_wifi_connect;
} t_global_state;


// Function declarations
void updateScheduler(t_scheduler_action action);

void setupWifiConnect(void);
void setupWebServerFiles(void);
void setupWebServer(void);
void setupTemperature(void);
void setupHumidity(void);
void setupClock(void);

void taskWifiConnect(void);
void taskWebServer(void);
void taskTemperature(void);
void taskHumidity(void);
void taskClock(void);

void resetWifiConnect(void);
void resetWebServer(void);
void resetTemperature(void);
void resetHumidity(void);
void resetClock(void);

void temperatureMeanReadAndReset(void);
void humidityMeanReadAndReset(void);

/* Global reference variables */

extern schedulerTask *task_wifi_connect, *task_web_server, *task_measurements, *task_humidity, *task_clock;

extern ESP8266WebServer server;
extern WiFiUDP UdpInstance;
extern SHT21 Sht;
extern t_global_state state;

extern String FILE_PREFIX;
extern t_files FILE_SUFFIXES;


#endif // _SKETCH_H
