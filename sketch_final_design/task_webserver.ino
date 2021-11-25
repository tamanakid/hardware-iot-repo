#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "file_handler.h"
#include "hardware.h"
#include "hardware_d1mini.h"
#include "sketch.h"



extern t_global_state state;
extern ESP8266WebServer server;


String indexHTML;
String stylesCSS;


void setupWebServer() {
  Serial.println("task:webServer> Setup");

  pinMode(MINID1_PIN_D0, OUTPUT);
  digitalWrite(MINID1_PIN_D0, LOW);

  indexHTML = fileRead("/index.html");
  if (indexHTML.equals("Error")){
    Serial.println("Error reading 'index.html' from flash.");
    return;
  }
  
  server.on("/home", handleHome);
  // server.on("/set-alarm", handleSetAlarm);
  // server.on("/set-light-threshold", handleSetLightThreshold);
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
  const int chunk_size = 500;
  // char content[20000];

  // strcpy(content, indexHTML.c_str());
  // snprintf(temp,1000,temp1,hr, min % 60, sec % 60,  Vumbral, normalizar(valor_medio));

  Serial.println("Sending index file to client");

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);

  // int file_length = indexHTML.length();
  server.sendHeader("Content-Length", (String)indexHTML.length());
  server.send(200, "text/html", "");
    
  for (int i = 0; i*chunk_size < indexHTML.length(); i++) {
    String chunk;
    if (i*chunk_size < (indexHTML.length() - chunk_size)) {
      chunk = indexHTML.substring(i*chunk_size, i*chunk_size + chunk_size);
    } else {
      chunk = indexHTML.substring(i*chunk_size);
    }

    char content_buffer[chunk_size];
    strcpy(content_buffer, chunk.c_str());

    // Serial.println("chunk:");
    Serial.println(content_buffer);

    server.sendContent_P(content_buffer, chunk_size);
  }

  Serial.println("Index file sent!");
  
  // server.send_P(200, "text/html", content);
  // text/css;charset=UTF-8
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
