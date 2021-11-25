#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "hardware_d1mini.h"
#include "scheduler.h"
#include "sketch.h"
#include "ssid.h"


t_global_state state = {
  is_server_active: false,
  temperature: 0,
  humidity: 0,
  temperature_means: { 0, 0, 0 },
  humidity_means: { 0, 0, 0 },
  is_temperature_alarm: false,
  is_humidity_alarm: false,
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


schedulerTask *task_wifi_connect, *task_web_server, *task_measurements, *task_clock;


void setup() {
  Serial.begin(115200);

  analogReference(DEFAULT);
  
  Serial.println("");
  Serial.println("");
  Serial.println("RESET: Running setup function.");
  
  setupWifiConnect();
  setupMeasurements();

  task_wifi_connect = scheduler_add(5, &taskWifiConnect, true);
  task_web_server = scheduler_add(1, &taskWebServer, false);

  // TODO: Read actual rate from config file
  task_measurements = scheduler_add(50, &taskMeasurements, false);
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
      scheduler_activate(task_measurements);
      scheduler_activate(task_clock);
      break;

    case ACTION_ON_DISCONNECTED:
      Serial.println(">> Action: Disconnected");
      scheduler_deactivate(task_web_server);
      scheduler_deactivate(task_measurements);
      scheduler_deactivate(task_clock);
      resetMeasurements();
      resetWebServer();
      resetClock();
      break;
  }
  
  last_action = action;
}
