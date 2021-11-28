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
String scriptJS;


void setupWebServer() {
  Serial.println("task:webServer> Setup");

  pinMode(MINID1_PIN_D0, OUTPUT);
  digitalWrite(MINID1_PIN_D0, LOW);

  indexHTML = fileRead("/index.html");
  if (indexHTML.equals("Error")){
    Serial.println("Error reading 'index.html' from flash.");
    return;
  }
  
  stylesCSS = fileRead("/styles.css");
  if (stylesCSS.equals("Error")){
    Serial.println("Error reading 'styles.css' from flash.");
    return;
  }

  scriptJS = fileRead("/script.js");
  if (scriptJS.equals("Error")){
    Serial.println("Error reading 'script.js' from flash.");
    return;
  }
  
  server.on("/index.html", handleIndex);
  server.on("/styles.css", handleStyles);
  server.on("/script.js", handleScriptJS);
  
  server.on("/api/test", handleApiTest);
  server.on("/api/temperature", handleTemperatureRequest);
  
  server.on("/", []() {
    server.sendHeader("Location", String("/index.html"), true);
    server.send(302, "text/plain", "");
  });
  
  server.onNotFound(handleNotFound);

  Serial.println("task:webServer> Setup complete");
  // server.begin();
}


void taskWebServer() {
  if (!state.is_server_active) {
    Serial.println("task:webServer> Server started.");
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


void handleFile(String *file_content, int file_length, String content_type) {
  const int chunk_size = 500;

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);

  Serial.print("File length: ");
  Serial.println(file_length);
  
  server.sendHeader("Content-Length", (String)file_length);
  server.send(200, content_type, "");
    
  for (int i = 0; i*chunk_size < file_length; i++) {
    String chunk;
    char content_buffer[chunk_size];
    int content_size;
    
    if (i*chunk_size < (file_length - chunk_size)) {
      chunk = file_content->substring(i*chunk_size, i*chunk_size + chunk_size);
      content_size = chunk_size;
    } else {
      chunk = file_content->substring(i*chunk_size);
      content_size = file_length - i*chunk_size;
    }
    
    strcpy(content_buffer, chunk.c_str());

    // Serial.println("chunk:");
    Serial.println(content_buffer);

    server.sendContent_P(content_buffer, content_size);
  }

  Serial.println("File sent!");
}


void handleIndex () {
  Serial.println("Sending index.html");
  int file_length = indexHTML.length();
  handleFile(&indexHTML, file_length, "text/html");
}

void handleStyles () {
  Serial.println("Sending styles.css");
  int file_length = stylesCSS.length();
  handleFile(&stylesCSS, file_length, "text/css;charset=UTF-8");
}

void handleScriptJS () {
  Serial.println("Sending script.js");
  int file_length = scriptJS.length();
  handleFile(&scriptJS, file_length, "application/javascript;charset=UTF-8");
}



void handleApiTest () {
  char json[100];

  strcpy(json, "{\"test-1\": 5, \"test-2\": \"22/11/2021\"}");

  server.send(200, "application/json", json);
}


void handleTemperatureRequest () {
  char json[1000];

  sprintf(json,
    "{\"temperature\": { \"current\": { \"value\": %2.1f }, \"means\": [{ \"value\": %2.1f }, { \"value\": %2.1f }, { \"value\": %2.1f }] },\
    \"humidity\": { \"current\": { \"value\": %d }, \"means\": [{ \"value\": %d }, { \"value\": %d }, { \"value\": %d }] }}\
    ",
    state.temperature,
    state.temperature_means[0],
    state.temperature_means[1],
    state.temperature_means[2],
    state.humidity,
    state.humidity_means[0],
    state.humidity_means[1],
    state.humidity_means[2]
  );

  server.send(200, "application/json", json);
}


// TODO: separate temperature and humidity requests in client-side code
void handleHumidityRequest () {
  char json[500];

  sprintf(json,
    "{\"humidity\": { \"current\": { \"value\": %2.1f }, \"means\": [{ \"value\": %2.1f }, { \"value\": %2.1f }, { \"value\": %2.1f }] }}",
    state.humidity,
    state.humidity_means[0],
    state.humidity_means[1],
    state.humidity_means[2]
  );

  server.send(200, "application/json", json);
}



extern schedulerTask *task_temperature, *task_humidity;

void handleChangeTemperatureRate () {
  
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
