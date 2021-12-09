#include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
// #include <WiFiUdp.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "hardware.h"
#include "hardware_d1mini.h"
#include "sketch.h"



extern t_global_state state;


int last_wifi_status;
int wifi_led = HIGH; // Keep in mind: on when LOW, off when HIGH

unsigned int udp_port_http = 80;
WiFiUDP UdpInstance;

// ESP8266WebServer server = ESP8266WebServer(udp_port_http);
AsyncWebServer server(udp_port_http);



void setupWifiConnect () {
  pinMode(MINID1_PIN_D4, OUTPUT);
  digitalWrite(MINID1_PIN_D4, wifi_led);

  WiFi.mode(WIFI_STA);
   
  WiFi.begin(STASSID, STAPSK);
}


void taskWifiConnect() {
  int status = WiFi.status();
  
  if (last_wifi_status != status) {
    switch (status) {
      case WL_IDLE_STATUS:
        Serial.println("task:WifiConnect> Idle Status");
        break;
        
      case WL_CONNECTED:
        Serial.print("task:WifiConnect> Connected to IP address: ");
        Serial.println(WiFi.localIP());
        Serial.printf("task:WifiConnect> UdpInstance server on port %d\n", udp_port_http);
        UdpInstance.begin(udp_port_http);

        wifi_led = LOW;
        updateScheduler(ACTION_ON_CONNECTED);

        if (!state.is_first_wifi_connect) {
          String log_string = "Connected to WiFi."; // TODO: WiFi name?
          fileWriteWithTimestamp(state.current_files.file_log, log_string, &state.time_clock);
        }
        break;

      case WL_CONNECT_FAILED:
        Serial.println("task:WifiConnect> Connection failed.");
        
        wifi_led = HIGH;
        break;

      case WL_CONNECTION_LOST:
        Serial.println("task:WifiConnect> Connection Lost.");
        
        wifi_led = HIGH;

        if (!state.is_first_wifi_connect) {
          String log_string = "Disconnected from WiFi (Connection Lost)."; // TODO: WiFi name?
          fileWriteWithTimestamp(state.current_files.file_log, log_string, &state.time_clock);
        }
        break;

      case WL_NO_SSID_AVAIL:
        Serial.println("task:WifiConnect> The specified SSID is not available");
        
        wifi_led = HIGH;
        break;

      case WL_DISCONNECTED:
        Serial.println("task:WifiConnect> Disconnected from Access Point.");
        
        wifi_led = HIGH;
        updateScheduler(ACTION_ON_DISCONNECTED);

        if (!state.is_first_wifi_connect) {
          String log_string = "Disconnected from WiFi."; // TODO: WiFi name?
          fileWriteWithTimestamp(state.current_files.file_log, log_string, &state.time_clock);
        }
        break;
    }
  }
  // else if block for LED blinking when trying to connect
  else if (status == WL_DISCONNECTED) {
    wifi_led = (wifi_led == HIGH) ? LOW : HIGH;
  }

  digitalWrite(MINID1_PIN_D4, wifi_led);
  
  last_wifi_status = status;
}

void resetWifiConnect() {
  
}
