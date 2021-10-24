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
  char temp[800];

  Serial.println(state.light);

  snprintf(temp, 800,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='5'/>\
        <link rel='icon' type='image/png' href='https://freepngimg.com/download/technology/51071-7-microcontroller-hd-image-free-png.png'/>\
        <title>ESP8266 Interface</title>\
        <style>\
          body { background-color: #024; color: #def; margin: 2em; font-size: 1.5em; display: flex; flex-direction: column; align-items: center; }\
          button { background: #fee; padding: 1em; border: 2px solid #d77; border-radius: 1em; font-size: 1.25em }\
          a { text-decoration: none; }\
        </style>\
      </head>\
      <body>\
        <h1>ESP8266 Interface</h1>\
        <h2>Light sensor: %04d</h2>\
        <a href='/set-alarm'><button style='display: %s'>Activate alarm</button></a>\
      </body>\
    </html>",
    
    state.light,
    state.is_overlit ? "inherit" : "none"
  );
  server.send(200, "text/html", temp);
}

void handleSetAlarm() {
  char temp[600];

  digitalWrite(MINID1_PIN_D0, HIGH);

  snprintf(temp, 600,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='5; url=\"/home\"'/>\
        <title>ESP8266 Interface</title>\
        <style>\
          body { background-color: #024; color: #def; text-align: center; margin: 2em; font-size: 1.5em; }\
        </style>\
      </head>\
      <body>\
        <h1>ESP8266 Interface</h1>\
        <h2>Light sensor: %04d</h2>\
        <h2>ALARM SET!</h2>\
      </body>\
    </html>",
    
    state.light
  );
  server.send(200, "text/html", temp);
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
