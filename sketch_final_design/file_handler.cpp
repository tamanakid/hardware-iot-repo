#include "FS.h"


String fileRead (String fileURI) {
    String myString;
    
    if (!SPIFFS.begin()) {
      Serial.println("Error on SPIFFS.begin()");
      return "Error";
    }

    if (SPIFFS.exists(fileURI)) {
      Serial.println("File exists in flash");
    } else {
      Serial.println("File doesn't exist in flash");
    }
    
    File file = SPIFFS.open(fileURI, "r");
    if (!file) {
      Serial.println("Error opening file");
      return "Error";
    }

    Serial.println("Reading file...");
    while (file.available()) {
      myString += file.readStringUntil('\n');
    }
    Serial.println("File read successfully");
    
    file.close();
    
    return myString;
}
