#include "FS.h"

#include "sketch.h"


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



bool fileWrite (String fileURI, String contents) {
  File file = SPIFFS.open(fileURI, "a");
  
  if (!file) {
    Serial.print("Error opening file for append: ");
    Serial.println(fileURI);
    return false;
  }

  file.println(contents + "\n");
  // file.write((contents + "\n").c_str(), contents.length() + 1);

  file.close();

  return true;
}

bool fileOverwrite (String fileURI, String contents) {
  File file = SPIFFS.open(fileURI, "w");
  
  if (!file) {
    Serial.print("Error opening file for append: ");
    Serial.println(fileURI);
    return false;
  }

  file.println(contents + "\n");
  // file.write((contents + "\n").c_str(), contents.length() + 1);

  file.close();

  return true;
}


bool fileWriteWithTimestamp (String fileURI, String contents, struct tm *state_time) {
  char time_char[11];
  sprintf(time_char, "%02d:%02d%:%02d - ", state_time->tm_hour, state_time->tm_min, state_time->tm_sec);
  String time_string = (String)time_char;

  String contents_with_timestamp = time_string + contents;
  return fileWrite(fileURI, contents_with_timestamp);
}



void fileSetup(String *state_filename, String file_suffix, struct tm *state_time) {
  char date_char[8];
  sprintf(date_char, "%02d%02d%02d", state_time->tm_year % 100, state_time->tm_mon, state_time->tm_mday);
  String date_string = (String)date_char;
  
  String new_filename = FILE_PREFIX + date_string + file_suffix;
  
  bool is_same_filename = new_filename.equals(*state_filename);
  bool does_file_exist = SPIFFS.exists(new_filename);

  if (!is_same_filename) {
    *state_filename = new_filename;
  }
  
  if (!does_file_exist) {
    String first_line = "File '" + new_filename + "' created.";
    
    bool success = fileWriteWithTimestamp(*state_filename, first_line, state_time);
    if (success) {
      Serial.print("File created: ");
      Serial.println(*state_filename);
    }  
  }
}
