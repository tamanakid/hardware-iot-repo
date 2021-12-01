#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SHT21.h>

#include "hardware_d1mini.h"
#include "scheduler.h"
#include "sketch.h"
#include "ssid.h"


SHT21 Sht;

t_global_state state = {
  temperature: {
    current: {
      value: NULL,
      timestamp: "",
    },
    mean_current: 0,
    mean_count: 0,
    means: { NULL, NULL, NULL },
    record_min: {
      value: NULL,
      timestamp: "",
    },
    record_max: {
      value: NULL,
      timestamp: "",
    },
    threshold: 30,
    is_alarm: false,
  },
  humidity: {
    current: {
      value: 0,
      timestamp: "",
    },
    mean_current: 0,
    mean_count: 0,
    means: { 0, 0, 0 },
    record_min: {
      value: NULL,
      timestamp: "",
    },
    record_max: {
      value: NULL,
      timestamp: "",
    },
    threshold: 50,
    is_alarm: false,
  },
  is_server_active: false,
  timestamp: {
    hours: 0,
    minutes: 0,
    seconds: 0,
    epoch: 0,
  }
  /*
  measures: {
    values: { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    count: 0,
  }
  */
};


schedulerTask *task_wifi_connect, *task_web_server, *task_temperature, *task_humidity, *task_clock;


void setup() {
  Serial.begin(115200);

  analogReference(DEFAULT);
  
  Serial.println("");
  Serial.println("");
  Serial.println("RESET: Running setup function.");
  
  setupWifiConnect();
  setupTemperature();
  setupHumidity();

  task_wifi_connect = scheduler_add(5, &taskWifiConnect, true);
  task_web_server = scheduler_add(1, &taskWebServer, false);

  // TODO: Read actual rate from config file
  task_temperature = scheduler_add(50, &taskTemperature, false);
  task_humidity = scheduler_add(50, &taskHumidity, false);
  task_clock = scheduler_add(10, &taskClock, false);
}


void loop() {
  scheduler_run();
  delay(100);
}


// TODO: Maybe break into methods in order to support further arguments
void updateScheduler (t_scheduler_action action) {
  static t_scheduler_action last_action = ACTION_ON_DISCONNECTED;

  if (last_action == action) {
    return;
  }
  
  switch (action) {
    case ACTION_ON_CONNECTED:
      Serial.println(">> Action: Connected");
      setupWebServer();
      setupClock();
      scheduler_activate(task_web_server);
      scheduler_activate(task_temperature);
      scheduler_activate(task_humidity);
      scheduler_activate(task_clock);
      break;

    case ACTION_ON_DISCONNECTED:
      Serial.println(">> Action: Disconnected");
      scheduler_deactivate(task_web_server);
      scheduler_deactivate(task_temperature);
      scheduler_deactivate(task_humidity);
      scheduler_deactivate(task_clock);
      resetWebServer();
      resetTemperature();
      resetHumidity();
      resetClock();
      break;
  }
  
  last_action = action;
}
