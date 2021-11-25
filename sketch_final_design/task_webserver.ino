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
  Serial.print("File length:");
  Serial.println(indexHTML.length());
  
  stylesCSS = fileRead("/styles.css");
  if (stylesCSS.equals("Error")){
    Serial.println("Error reading 'styles.css' from flash.");
    return;
  }
  
  server.on("/index.html", handleIndex);
  server.on("/styles.css", handleStyles);
  // server.on("/set-alarm", handleSetAlarm);
  // server.on("/set-light-threshold", handleSetLightThreshold);
  server.on("/", []() {
    server.sendHeader("Location", String("/index.html"), true);
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
