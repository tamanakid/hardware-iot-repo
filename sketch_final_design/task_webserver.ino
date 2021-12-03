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

  server.on("/api/temp", handleTemperatureRequest);
  server.on("/api/humi", handleHumidityRequest);

  server.on("/api/temp/rate", handleChangeTemperatureRate);
  server.on("/api/humi/rate", handleChangeHumidityRate);
  
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



extern schedulerTask *task_temperature, *task_humidity;


void handleTemperatureRequest () {
  char json[1000];
  
  char string_mean_0[4];
  char string_mean_1[4];
  char string_mean_2[4];
  sprintf(string_mean_0, "%2.1f", state.temperature.means[0]);
  sprintf(string_mean_1, "%2.1f", state.temperature.means[1]);
  sprintf(string_mean_2, "%2.1f", state.temperature.means[2]);

  sprintf(json,
    "{\
    \"current\": { \"value\": %2.1f, \"timestamp\": \"%s\"  },\
    \"means\": [{ \"value\": %s }, { \"value\": %s }, { \"value\": %s }],\
    \"config\": { \"rate\": %d, \"threshold\": %2.1f },\
    \"alarm\": %s\
    }",
    state.temperature.current.value,
    state.temperature.current.timestamp,
    state.temperature.means[0] == NULL ? "null" : string_mean_0,
    state.temperature.means[1] == NULL ? "null" : string_mean_1,
    state.temperature.means[2] == NULL ? "null" : string_mean_2,
    task_temperature->schedule_ticks / 10,
    state.temperature.threshold,
    state.temperature.is_alarm ? "true" : "false"
  );

  server.send(200, "application/json", json);
}


void handleHumidityRequest () {
  char json[500];

  sprintf(json,
    "{\
    \"current\": { \"value\": %d, \"timestamp\": \"%s\"  },\
    \"means\": [{ \"value\": %d }, { \"value\": %d }, { \"value\": %d }],\
    \"config\": { \"rate\": %d, \"threshold\": %d },\
    \"alarm\": %s\
    }",
    state.humidity.current.value,
    state.humidity.current.timestamp,
    state.humidity.means[0],
    state.humidity.means[1],
    state.humidity.means[2],
    task_humidity->schedule_ticks / 10,
    state.humidity.threshold,
    state.humidity.is_alarm ? "true" : "false"
  );

  server.send(200, "application/json", json);
}


void handleChangeTemperatureRate () {
  char json[500];
  
  if (server.hasArg("value")) {
    int new_rate = server.arg("value").toInt();
    
    task_temperature->schedule_ticks = new_rate*10;
    
    sprintf(json, "{ \"success\": true, \"value\": %d }", new_rate);
  } else {
    int seconds = task_temperature->schedule_ticks / 10;
    sprintf(json, "{ \"success\": false, \"value\": %d, \"message\": \"The 'value' parameter is missing from the request.\" }", seconds);
  }

  server.send(200, "application/json", json);
}

void handleChangeHumidityRate () {
  char json[500];
  
  if (server.hasArg("value")) {
    int new_rate = server.arg("value").toInt();
    
    task_humidity->schedule_ticks = new_rate*10;
    
    sprintf(json, "{ \"success\": true, \"value\": %d }", new_rate);
  } else {
    int seconds = task_humidity->schedule_ticks / 10;
    sprintf(json, "{ \"success\": false, \"value\": %d, \"message\": \"The 'value' parameter is missing from the request.\" }", seconds);
  }

  server.send(200, "application/json", json);
}


void handleNotFound() {
  char json[500];

  sprintf(json,
    "{ \"message\": \"endpoint not found\", \"URI\": \"%s\", \"method\": \"%s\", \"args\": \"%s\" }",
    server.uri(),
    server.method() == HTTP_GET ? "GET" : "POST",
    server.args()
  );
  
  server.send(404, "application/json", json);
}
