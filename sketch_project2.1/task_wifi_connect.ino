#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "hardware.h"
#include "hardware_d1mini.h"
#include "sketch.h"



extern t_global_state state;


int last_status;
int builtin_led = HIGH; // Keep in mind: on when LOW, off when HIGH

unsigned int udp_port = 80;
WiFiUDP Udp;

ESP8266WebServer server = ESP8266WebServer(udp_port);


void setupWifiConnect () {
  pinMode(MINID1_PIN_D4, OUTPUT);
  digitalWrite(MINID1_PIN_D4, builtin_led);

  WiFi.mode(WIFI_STA);
  
  WiFi.begin(STASSID, STAPSK);
  
  Serial.println("task:WifiConnect> Print Statuses");
  Serial.print("WL_CONNECTED");
  Serial.println(WL_CONNECTED);
  Serial.print("WL_NO_SHIELD");
  Serial.println(WL_NO_SHIELD);
  Serial.print("WL_IDLE_STATUS");
  Serial.println(WL_IDLE_STATUS);
  Serial.print("WL_NO_SSID_AVAIL");
  Serial.println(WL_NO_SSID_AVAIL);
  Serial.print("WL_SCAN_COMPLETED");
  Serial.println(WL_SCAN_COMPLETED);
  Serial.print("WL_CONNECT_FAILED");
  Serial.println(WL_CONNECT_FAILED);
  Serial.print("WL_DISCONNECTED");
  Serial.println(WL_DISCONNECTED);
  Serial.print("WL_CONNECTION_LOST");
  Serial.println(WL_CONNECTION_LOST);
}


void taskWifiConnect() {
  int status = WiFi.status();
  
  if (last_status != status) {
    switch (status) {
      case WL_IDLE_STATUS:
        Serial.println("task:WifiConnect> Idle Status");
        break;
        
      case WL_CONNECTED:
        Serial.print("task:WifiConnect> Connected to IP address: ");
        Serial.println(WiFi.localIP());
        Serial.printf("task:WifiConnect> UDP server on port %d\n", udp_port);
        Udp.begin(udp_port);

        builtin_led = LOW;
        setSchedulerState(ACTION_ON_CONNECTED);
        break;

      case WL_CONNECT_FAILED:
        Serial.println("task:WifiConnect> Connection failed.");
        
        builtin_led = HIGH;
        break;

      case WL_CONNECTION_LOST:
        Serial.println("task:WifiConnect> Connection Lost.");
        
        builtin_led = HIGH;
        break;

      case WL_NO_SSID_AVAIL:
        Serial.println("task:WifiConnect> The specified SSID is not available");
        
        builtin_led = HIGH;
        break;

      case WL_DISCONNECTED:
        Serial.println("task:WifiConnect> Disconnected from Access Point.");
        
        builtin_led = HIGH;
        setSchedulerState(ACTION_ON_DISCONNECTED);
        break;
    }
  }
  // else if block for LED blinking when trying to connect
  else if (status == WL_DISCONNECTED) {
    builtin_led = (builtin_led == HIGH) ? LOW : HIGH;
  }

  digitalWrite(MINID1_PIN_D4, builtin_led);
  
  last_status = status;
}

void resetWifiConnect() {
  
}
