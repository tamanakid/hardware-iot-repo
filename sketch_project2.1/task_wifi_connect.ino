#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "hardware_d1mini.h"
#include "sketch.h"



int last_status;

unsigned int udp_port = 80;
WiFiUDP Udp;


void setupWifiConnect () {
  pinMode(MINID1_PIN_D4, OUTPUT);
  digitalWrite(MINID1_PIN_D4, HIGH);

  WiFi.mode(WIFI_STA);
  
  WiFi.begin(STASSID, STAPSK);
}


void taskWifiConnect() {
  int status = WiFi.status();

  if (last_status != status) {
    switch (status) {
      case WL_IDLE_STATUS:
        Serial.println("task:WifiConnect> Attempting to connect...");
        break;
        
      case WL_CONNECTED:
        Serial.print("task:WifiConnect> Connected to IP address: ");
        Serial.println(WiFi.localIP());
        Serial.printf("task:WifiConnect> UDP server on port %d\n", udp_port);
        Udp.begin(udp_port);

        digitalWrite(MINID1_PIN_D4, LOW);
        setSchedulerState(ACTION_BUZZER_ACTIVATE);
        break;

      case WL_CONNECT_FAILED:
        Serial.println("task:WifiConnect> Connection failed.");
        break;

      case WL_DISCONNECTED:
        Serial.println("task:WifiConnect> Disconnected from Access Point.");
        
        digitalWrite(MINID1_PIN_D4, HIGH);
        setSchedulerState(ACTION_BUZZER_DEACTIVATE);
        break;
    }
  }

  last_status = status;
}
