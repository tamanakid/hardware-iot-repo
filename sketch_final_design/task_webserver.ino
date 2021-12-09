// #include <ESP8266WebServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"

#include "file_handler.h"
#include "hardware.h"
#include "hardware_d1mini.h"
#include "sketch.h"



extern t_global_state state;
// extern ESP8266WebServer server;
extern AsyncWebServer server;

extern schedulerTask *task_temperature, *task_humidity;

String indexHTML;
String stylesCSS;
String scriptJS;


void setupWebServerFiles() {
  indexHTML = fileRead("/ui/index.html");
  if (indexHTML.equals("Error")){
    Serial.println("Error reading 'index.html' from flash.");
    return;
  }
  
  stylesCSS = fileRead("/ui/styles.css");
  if (stylesCSS.equals("Error")){
    Serial.println("Error reading 'styles.css' from flash.");
    return;
  }

  scriptJS = fileRead("/ui/script.js");
  if (scriptJS.equals("Error")){
    Serial.println("Error reading 'script.js' from flash.");
    return;
  }
}


void setupWebServer() {
  Serial.println("task:webServer> Setup");

  pinMode(MINID1_PIN_D0, OUTPUT);
  digitalWrite(MINID1_PIN_D0, LOW);

  server.on("/index.html", HTTP_GET, handleIndex);
  server.on("/styles.css", HTTP_GET, handleStyles);
  server.on("/script.js", HTTP_GET, handleScriptJS);

  server.on("/api/temp", HTTP_GET, handleTemperatureRequest);
  server.on("/api/humi", HTTP_GET, handleHumidityRequest);

  server.on("/api/temp/rate", HTTP_POST, handleChangeTemperatureRate);
  server.on("/api/humi/rate", HTTP_POST, handleChangeHumidityRate);

  server.on("/api/temp/thres", HTTP_POST, handleChangeTemperatureThreshold);
  server.on("/api/humi/thres", HTTP_POST, handleChangeHumidityThreshold);

  server.on("/api/clock/get", HTTP_GET, handleClockGet);
  server.on("/api/clock/set", HTTP_POST, handleClockSet);

  server.on("/api/files/all", HTTP_GET, handleFilesGetAll);
  server.on("/api/files/get", HTTP_POST, handleFilesGet);
  server.on("/api/files/delete", HTTP_GET, handleFilesDelete);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // request->addHeader("Location", String("/index.html"), true);
    // request->send(302, "text/plain", "");
    request->redirect("/index.html");
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

  // server.handleClient();
}


void resetWebServer() {
  // server.stop();
  state.is_server_active = false;
  digitalWrite(MINID1_PIN_D0, LOW);
}


void handleFile(String *file_content, int file_length, String content_type, AsyncWebServerRequest *request) {
  const int chunk_size = 500;

  // server.setContentLength(CONTENT_LENGTH_UNKNOWN);

  Serial.print("File length: ");
  Serial.println(file_length);
  
  // request->addHeader("Content-Length", (String)file_length);
  // server.send(200, content_type, "");

  /*
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
  */
  AsyncWebServerResponse *response = request->beginResponse_P(200, content_type, file_content->c_str());
  response->addHeader("Content-Length", (String)file_length);
  request->send(response);

  Serial.println("File sent!");
}


void handleIndex (AsyncWebServerRequest *request) {
  // TODO: Get IP address for logging
  
  Serial.println("Sending index.html");
  int file_length = indexHTML.length();
  handleFile(&indexHTML, file_length, "text/html", request);
}

void handleStyles (AsyncWebServerRequest *request) {
  Serial.println("Sending styles.css");
  int file_length = stylesCSS.length();
  handleFile(&stylesCSS, file_length, "text/css;charset=UTF-8", request);
}

void handleScriptJS (AsyncWebServerRequest *request) {
  Serial.println("Sending script.js");
  int file_length = scriptJS.length();
  handleFile(&scriptJS, file_length, "application/javascript;charset=UTF-8", request);
}


void handleTemperatureRequest (AsyncWebServerRequest *request) {
  char json[1000];
  
  char string_mean_0[6];
  char string_mean_1[6];
  char string_mean_2[6];
  sprintf(string_mean_0, "%2.1f", state.temperature.means[0]);
  sprintf(string_mean_1, "%2.1f", state.temperature.means[1]);
  sprintf(string_mean_2, "%2.1f", state.temperature.means[2]);

  /*
  Serial.print("Latest temp means: ");
  Serial.print(string_mean_0);
  Serial.print(" ");
  Serial.print(string_mean_1);
  Serial.print(" ");
  Serial.println(string_mean_2);
  */

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

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}


void handleHumidityRequest (AsyncWebServerRequest *request) {
  char json[500];

  char string_mean_0[6];
  char string_mean_1[6];
  char string_mean_2[6];
  sprintf(string_mean_0, "%d", state.humidity.means[0]);
  sprintf(string_mean_1, "%d", state.humidity.means[1]);
  sprintf(string_mean_2, "%d", state.humidity.means[2]);

  sprintf(json,
    "{\
    \"current\": { \"value\": %d, \"timestamp\": \"%s\"  },\
    \"means\": [{ \"value\": %s }, { \"value\": %s }, { \"value\": %s }],\
    \"config\": { \"rate\": %d, \"threshold\": %d },\
    \"alarm\": %s\
    }",
    state.humidity.current.value,
    state.humidity.current.timestamp,
    state.humidity.means[0] == NULL ? "null" : string_mean_0,
    state.humidity.means[1] == NULL ? "null" : string_mean_1,
    state.humidity.means[2] == NULL ? "null" : string_mean_2,
    task_humidity->schedule_ticks / 10,
    state.humidity.threshold,
    state.humidity.is_alarm ? "true" : "false"
  );

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}


void handleChangeTemperatureRate (AsyncWebServerRequest *request) {
  char json[500];
  
  if (request->hasHeader("value")) {
    int new_rate = request->header("value").toInt();
    
    task_temperature->schedule_ticks = new_rate*10;
    
    sprintf(json, "{ \"success\": true, \"value\": %d }", new_rate);
  } else {
    int seconds = task_temperature->schedule_ticks / 10;
    sprintf(json, "{ \"success\": false, \"value\": %d, \"message\": \"The 'value' parameter is missing from the request.\" }", seconds);
  }

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}

void handleChangeHumidityRate (AsyncWebServerRequest *request) {
  char json[500];
  
  if (request->hasHeader("value")) {
    int new_rate = request->header("value").toInt();
    
    task_humidity->schedule_ticks = new_rate*10;
    
    sprintf(json, "{ \"success\": true, \"value\": %d }", new_rate);
  } else {
    int seconds = task_humidity->schedule_ticks / 10;
    sprintf(json, "{ \"success\": false, \"value\": %d, \"message\": \"The 'value' parameter is missing from the request.\" }", seconds);
  }

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}


void handleChangeTemperatureThreshold (AsyncWebServerRequest *request) {
  char json[500];
  
  if (request->hasHeader("value")) {
    int new_threshold = request->header("value").toInt();

    state.temperature.threshold = new_threshold;
    
    sprintf(json, "{ \"success\": true, \"value\": %d }", new_threshold);
  } else {
    int threshold = (int) state.temperature.threshold;
    sprintf(json, "{ \"success\": false, \"value\": %d, \"message\": \"The 'value' parameter is missing from the request.\" }", threshold);
  }

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}

void handleChangeHumidityThreshold (AsyncWebServerRequest *request) {
  char json[500];
  
  if (request->hasHeader("value")) {
    int new_threshold = request->header("value").toInt();

    state.humidity.threshold = new_threshold;
    
    sprintf(json, "{ \"success\": true, \"value\": %d }", new_threshold);
  } else {
    int threshold = state.humidity.threshold;
    sprintf(json, "{ \"success\": false, \"value\": %d, \"message\": \"The 'value' parameter is missing from the request.\" }", threshold);
  }

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}


void handleClockGet (AsyncWebServerRequest *request) {
  char json[500];

  int hour = state.time_clock.tm_hour;
  int minute = state.time_clock.tm_min;
  
  sprintf(json, "{ \"hour\": \"%02d\", \"minute\": \"%02d\" }", hour, minute);

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}


void handleClockSet (AsyncWebServerRequest *request) {
  char json[500];
  
  if (request->hasHeader("hour") && request->hasHeader("minute")) {
    int hour = request->header("hour").toInt();
    int minute = request->header("minute").toInt();

    state.time_clock.tm_hour = hour;
    state.time_clock.tm_min = minute;
    state.time_clock.tm_sec = 0;
    
    sprintf(json, "{ \"success\": true, \"value\": { \"hour\": \"%02d\", \"minute\": \"%02d\" } }", hour, minute);
    
  } else {
    int hour = state.time_clock.tm_hour;
    int minute = state.time_clock.tm_min;
    sprintf(json, "{ \"success\": false, \"value\": { \"hour\": \"%02d\", \"minute\": \"%02d\" }, \"message\": \"The 'hour' and/or 'minute' parameters are missing from the request.\" }", hour, minute);
  }

  // server.send(200, "application/json", json);
  request->send(200, "application/json", json);
}



void handleFilesGetAll(AsyncWebServerRequest *request) {
  char json[5000];
  sprintf(json, "[");
  
  Dir dir = SPIFFS.openDir("");

  int count = 0;
  char filename_char[50];
  
  while (dir.next()) {
    String filename = dir.fileName();
    if (filename.startsWith("/ui/")) {
      continue;
    }
    
    filename_char[0] = 0;
    if (count > 0) {
      strcat(json, ",");
    }
    count++;

    // Serial.print("File found: ");
    // Serial.print(filename);
    // Serial.print(" - Size: ");
    // Serial.println (dir.fileSize());

    sprintf(filename_char, "\"%s\"", filename.c_str());    
    strcat(json, filename_char);
  }
  strcat(json, "]");

  // server.send(200, "application/json;charset=UTF-8", json);
  request->send(200, "application/json;charset=UTF-8", json);
}

void handleFilesGet(AsyncWebServerRequest *request) {
  String json;

  if (request->hasHeader("filename")) {
    String filename = request->header("filename");

    if (!SPIFFS.exists(filename)) {
      json = "{ \"success\": false, \"message\": \"File not found: ";
      json += filename;
      json += "\" }";
      
    } else {
      Serial.print("Reading file: ");
      Serial.println(filename);
      
      String content = fileRead(filename);
      // content.replace("\n", "\\n");
      content.replace("\r", "\\n");

      Serial.print("File read!");

      json = "{ \"success\": true, \"content\": \"";
      json += content;
      json += "\" }";
      // sprintf(json, "{ \"success\": true, \"content\": %d }", content);
    }
  } else {
    json = "{ \"success\": false, \"message\": \"The 'filename' parameter is missing from the request.\" }";
  }

  // server.send(200, "application/json;charset=UTF-8", json);
  handleFile(&json, json.length(), "application/json;charset=UTF-8", request);
}


void handleFilesDelete(AsyncWebServerRequest *request) {
  Dir dir = SPIFFS.openDir("");

  while (dir.next()) {
    String filename = dir.fileName();
    if (filename.startsWith("/ui/")) {
      continue;
    }

    SPIFFS.remove(filename);
  }

  handleFilesGetAll(request);
}



void handleNotFound(AsyncWebServerRequest *request) {
  char json[500];

  sprintf(json,
    "{ \"message\": \"endpoint not found\", \"URI\": \"%s\", \"method\": \"%s\", \"args\": \"%s\" }",
    request->url(), // server.uri(),
    request->method() == HTTP_GET ? "GET" : "POST", // server.method() == HTTP_GET ? "GET" : "POST",
    request->headers() // server.args()
 );
  
  // server.send(404, "application/json", json);
  request->send(404, "application/json", json);
}
