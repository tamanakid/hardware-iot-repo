#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "hardware.h"
#include "hardware_d1mini.h"
#include "sketch.h"



extern t_global_state state;
extern ESP8266WebServer server;


void setupWebServer() {
  pinMode(MINID1_PIN_D0, OUTPUT);
  digitalWrite(MINID1_PIN_D0, LOW);
  
  server.on("/home", handleHome);
  server.on("/set-alarm", handleSetAlarm);
  server.on("/set-light-threshold", handleSetLightThreshold);
  server.on("/", []() {
    server.sendHeader("Location", String("/home"), true);
    server.send(302, "text/plain", "");
  });
  
  server.onNotFound(handleNotFound);
  // server.begin();
}


void taskWebServer() {
  if (!state.is_server_active) {
    server.begin();
    state.is_server_active = true;
  }

  server.handleClient();
}


void resetWebServer() {
  server.stop();
  state.is_server_active = false;
  digitalWrite(MINID1_PIN_D0, LOW);
}


void handleHome() {
  char temp[1600];
  
  snprintf(temp, 1600,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='5'/>\
        <link rel='icon' type='image/png' href='https://freepngimg.com/download/technology/51071-7-microcontroller-hd-image-free-png.png'/>\
        <title>ESP8266 Interface</title>\
        <style>\
          body { background-color: #024; color: #def; margin: 2em; font-size: 1.5em; display: flex; flex-direction: column; align-items: center; }\
          button { background: #fee; padding: 1em; border: 2px solid #d77; border-radius: 1em; font-size: 1.25em }\
          div.alarm-led { width: 50px; height: 50px; border-radius: 50px; border: 1em double #fff; margin: 1em }\
          div.alarm-led-on { background: #f00 }\
          div.alarm-led-off { background: none }\
          a { text-decoration: none; }\
        </style>\
      </head>\
      <body>\
        <h1>ESP8266 Interface</h1>\
        <h2>Light sensor: %04d mV</h2>\
        <form action=\"/set-alarm\" method=\"POST\">\
          <input type=\"submit\" value=\"Activate Alarm\" style=\"display: %s\">\
        </form>\
        <form action=\"/set-light-threshold\" method=\"POST\">\
          <label for=\"threshold\">Light sensor threshold</label>\
          <input type=\"number\" name=\"threshold\" value=\"%04d\" id=\"threshold\" placeholder=\"Insert a number between 0 and 1024\">\
          <input type=\"submit\" value=\"Submit\">\
        </form>\
        <div class=\"alarm-led %s\">\
      </body>\
    </html>",
    
    state.light,
    (state.is_overlit && !state.is_alarm_active) ? "inherit" : "none",
    state.light_threshold,
    state.is_alarm_active ? "alarm-led-on" : "alarm-led-off"
  );
  server.send(200, "text/html", temp);
}


void handleSetLightThreshold() {
  char* threshold = (char*) malloc(100*sizeof(char));
  sprintf(threshold, "%s", server.arg("threshold"));
  state.light_threshold = atoi(threshold);
  
  Serial.print("Setting new threshold: ");
  Serial.println(state.light_threshold);

  server.sendHeader("Location", String("/home"), true);
  server.send(302, "text/plain", "");
}


void handleSetAlarm() {
  digitalWrite(MINID1_PIN_D0, HIGH);
  state.is_alarm_active = true;

  server.sendHeader("Location", String("/home"), true);
  server.send(302, "text/plain", "");
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}
