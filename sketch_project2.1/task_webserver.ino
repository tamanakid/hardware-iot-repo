#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "hardware.h"
#include "hardware_d1mini.h"
#include "sketch.h"



extern t_global_state state;
extern ESP8266WebServer server;



void setupWebServer() {
  server.on("/", handleRoot);
  /*
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  */
  server.onNotFound(handleNotFound);
  // server.begin();
}


bool isActive = false;

void taskWebServer() {
  if (!isActive) {
    server.begin();
    isActive = true;
  }

  server.handleClient();
}


void resetWebServer() {
  server.stop();
  isActive = false;
}




void handleRoot() {
  char temp[600];

  Serial.println(state.light);

  snprintf(temp, 600,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='5'/>\
        <title>ESP8266 Interface</title>\
        <style>\
          body { background-color: #f0f7ff; color: #004499; text-align: center; margin: 2em; font-size: 1.5em; }\
        </style>\
      </head>\
      <body>\
        <h1>ESP8266 Interface</h1>\
        <h2>Light sensor: %04d</h2>\
        <button %s>Activate alarm</button>\
      </body>\
    </html>",
    
    state.light,
    state.is_overlit ? "" : "disabled"
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
