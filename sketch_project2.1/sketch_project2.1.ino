#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "hardware_d1mini.h"
#include "scheduler.h"
#include "sketch.h"


#ifndef STASSID
#define STASSID "tamanakid"
#define STAPSK  "password"
#endif

t_global_state state = {
  is_overlit: false,
  is_server_active: false,
  light: 0,
};

schedulerTask *task_wifi_connect, *task_buzzer, *task_light_sensor, *task_led_test, *task_web_server;


void setup() {
  Serial.begin(115200);
  
  setupWifiConnect();
  setupBuzzer();
  setupLightSensor();
  // setupLEDTest();
  setupWebServer();

  task_wifi_connect = scheduler_add(5, &taskWifiConnect, true);
  task_buzzer = scheduler_add(10, &taskBuzzer, false);
  task_light_sensor = scheduler_add(10, &taskLightSensor, false);
  // task_led_test = scheduler_add(2, &taskLEDTest, false);
  task_web_server = scheduler_add(1, &taskWebServer, false);
}


void loop() {
  scheduler_run();
  delay(100);
}


void setSchedulerState (t_state_action action) {
  static t_state_action last_action = ACTION_BUZZER_DEACTIVATE;

  if (last_action == action) {
    return;
  }
  
  switch (action) {
    case ACTION_ON_CONNECTED:
      Serial.println(">> Action: Connected");
      scheduler_activate(task_buzzer);
      scheduler_activate(task_light_sensor);
      // scheduler_activate(task_led_test);
      scheduler_activate(task_web_server);
      break;

    case ACTION_ON_DISCONNECTED:
      Serial.println(">> Action: Disconnected");
      scheduler_deactivate(task_buzzer);
      scheduler_deactivate(task_light_sensor);
      // scheduler_deactivate(task_led_test);
      scheduler_deactivate(task_web_server);
      resetBuzzer();
      resetLightSensor();
      // resetLEDTest();
      resetWebServer();
      break;

    case ACTION_BUZZER_DEACTIVATE:
      Serial.println(">> Action: Buzzer Deactivated");
      scheduler_deactivate(task_buzzer);
      resetBuzzer();
      break;
  }
  
  last_action = action;
}
