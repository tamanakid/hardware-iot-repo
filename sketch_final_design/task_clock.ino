#include <NTPClient.h>
#include "FS.h"
#include "time.h"

#include "file_handler.h"
#include "sketch.h"


extern t_global_state state;

extern WiFiUDP UdpInstance;

const int utcOffsetInSeconds = 3600;
NTPClient ntpClient(UdpInstance, "1.es.pool.ntp.org", utcOffsetInSeconds);


void updateNTP() {
  time_t timestamp;
  struct tm timestamp_struct;
  
  Serial.println("Requesting timestamp to NTP server...");
  ntpClient.update();
  
  timestamp = ntpClient.getEpochTime();
  timestamp_struct = *localtime(&timestamp);

  state.is_ntp_updated = state.time_clock.tm_hour == timestamp_struct.tm_hour &&
                         state.time_clock.tm_min  == timestamp_struct.tm_min  &&
                         state.time_clock.tm_sec  == timestamp_struct.tm_sec;
  
  state.time_clock = *localtime(&timestamp);

  state.time_clock.tm_year = state.time_clock.tm_year + 1900;
  state.time_clock.tm_mon = state.time_clock.tm_mon + 1;

  char ntp_time[50];
  sprintf(ntp_time, "%02d:%02d:%02d - %02d/%02d/%04d", state.time_clock.tm_hour, state.time_clock.tm_min, state.time_clock.tm_sec, state.time_clock.tm_mday, state.time_clock.tm_mon, state.time_clock.tm_year);
  Serial.print("Request timestamp to NTP server complete:");
  Serial.println(ntp_time);

  if (!state.is_ntp_updated) {
    setupLogFiles();
  }
}


void setupClock() {
  ntpClient.begin();

  updateNTP();
}


// TODO: Move to file_handler file?
String FILE_PREFIX = "STATION99_";
t_files FILE_SUFFIXES = {
  file_dat: ".dat",
  file_mdat: ".mdat",
  file_log: ".log",
};

void setupLogFiles() {
  char date_char[8];
  sprintf(date_char, "%02d%02d%02d", state.time_clock.tm_year % 100, state.time_clock.tm_mon, state.time_clock.tm_mday);
  String date_string = (String)date_char;
  // String date_string = ((String)state.time_clock.tm_year) + ((String)state.time_clock.tm_mon) + ((String)state.time_clock.tm_mday);
  
  setupFile(&state.current_files.file_dat, date_string, FILE_SUFFIXES.file_dat);
  setupFile(&state.current_files.file_mdat, date_string, FILE_SUFFIXES.file_mdat);
  setupFile(&state.current_files.file_log, date_string, FILE_SUFFIXES.file_log);
}

void setupFile(String *state_filename, String date_string, String file_suffix) {
  String new_filename = FILE_PREFIX + date_string + file_suffix;
  
  bool is_same_filename = new_filename.equals(*state_filename);
  bool does_file_exist = SPIFFS.exists(new_filename);

  if (!is_same_filename) {
    *state_filename = new_filename;
  }
  if (!does_file_exist) {
    char time_char[8];
    sprintf(time_char, "%02d:%02d%:%02d", state.time_clock.tm_hour, state.time_clock.tm_min, state.time_clock.tm_sec);
    // String time_string = ((String)state.time_clock.tm_hour) + ":" + ((String)state.time_clock.tm_min) + ":" + ((String)state.time_clock.tm_sec);
    String time_string = (String)time_char;
    String first_line = "File '" + new_filename + "' created at " + time_string;
    
    bool success = fileWrite(*state_filename, first_line);
    if (success) {
      Serial.print("File created: ");
      Serial.println(*state_filename);
    }
    
  }
}


void taskClock() {
  state.time_clock.tm_sec = state.time_clock.tm_sec + 1;
  
  if (state.time_clock.tm_sec == 60) {
    state.time_clock.tm_sec = 0;
    state.time_clock.tm_min = state.time_clock.tm_min + 1;
    // if state.timestamp.minutes % 5 == 0
    temperatureMeanReadAndReset();
    humidityMeanReadAndReset();
  }

  if (state.time_clock.tm_min == 60) {
    state.time_clock.tm_min = 0;
    state.time_clock.tm_hour = state.time_clock.tm_hour + 1;
  }

  if (state.time_clock.tm_hour == 24) {
    state.time_clock.tm_hour = 0;
    // call ntp request again
    if (!state.is_ntp_updated) {
      updateNTP();
    } else {
      setupLogFiles();
      state.is_ntp_updated = false;
    }
  }

  // debugging
  /*
  char *timestring = (char*) malloc(2*sizeof(char));
  sprintf(timestring, "%02d", state.timestamp.hours);
  Serial.print(timestring);
  Serial.print(":");
  sprintf(timestring, "%02d", state.timestamp.minutes);
  Serial.print(timestring);
  Serial.print(":");
  sprintf(timestring, "%02d", state.timestamp.seconds);
  Serial.println(timestring);
  */
}


void resetClock() {
  
}
