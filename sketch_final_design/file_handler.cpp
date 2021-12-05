#include "FS.h"


String fileRead (String fileURI) {
    String myString;
    
    if (!SPIFFS.begin()) {
      Serial.println("Error on SPIFFS.begin()");
      return "Error";
    }

    if (!SPIFFS.exists(fileURI)) {
      Serial.print("File doesn't exist in flash: ");
      Serial.println(fileURI);
      return "Error";
    }
    
    File file = SPIFFS.open(fileURI, "r");
    if (!file) {
      Serial.print("Error opening file: ");
      Serial.println(fileURI);
      return "Error";
    }

    Serial.print("Reading file: ");
    Serial.println(fileURI);
    while (file.available()) {
      myString += file.readStringUntil('\n');
    }
    Serial.println("File read successfully");
    
    file.close();
    
    return myString;
}
