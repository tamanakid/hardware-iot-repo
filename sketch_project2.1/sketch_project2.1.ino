#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "hardware_d1mini.h"
#include "scheduler.h"
#include "sketch.h"


#ifndef STASSID
#define STASSID "tamanakid"
#define STAPSK  "password"
#endif

schedulerTask *task_wifi_connect, *task_buzzer;


void setup() {
  Serial.begin(115200);
  
  setupWifiConnect();
  setupBuzzer();

  task_wifi_connect = scheduler_add(10, &taskWifiConnect, true);
  task_buzzer = scheduler_add(10, &taskBuzzer, false);
  
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
    case ACTION_BUZZER_ACTIVATE:
      Serial.println(">> State Change: Buzzer Activated");
      scheduler_activate(task_buzzer);
      break;

    case ACTION_BUZZER_DEACTIVATE:
      Serial.println(">> State Change: Buzzer Deactivated");
      scheduler_deactivate(task_buzzer);
      resetBuzzer();
      break;
  }
  
  last_action = action;
}
