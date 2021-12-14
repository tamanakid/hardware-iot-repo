#include <NTPClient.h>
#include "FS.h"
#include "time.h"

#include "file_handler.h"
#include "sketch.h"


extern t_global_state state;
extern t_files FILE_SUFFIXES;

extern schedulerTask *task_temperature, *task_humidity;

extern WiFiUDP UdpInstance;

const int utcOffsetInSeconds = 3600;
NTPClient ntpClient(UdpInstance, "1.es.pool.ntp.org", utcOffsetInSeconds);


void updateNTP() {
  time_t timestamp;
  struct tm timestamp_struct;

  do {
    Serial.println("Requesting timestamp to NTP server...");
    ntpClient.update();
    
    timestamp = ntpClient.getEpochTime();
    timestamp_struct = *localtime(&timestamp);
    
  } while (timestamp_struct.tm_year == 0);
  
  state.is_ntp_updated = state.time_clock.tm_hour == timestamp_struct.tm_hour &&
                         state.time_clock.tm_min  == timestamp_struct.tm_min  &&
                         state.time_clock.tm_sec  == timestamp_struct.tm_sec;
  
  state.time_clock = *localtime(&timestamp);

  state.time_clock.tm_year = state.time_clock.tm_year + 1900;
  state.time_clock.tm_mon = state.time_clock.tm_mon + 1;

  char ntp_time[50];
  sprintf(ntp_time, "%02d:%02d:%02d - %02d/%02d/%04d", state.time_clock.tm_hour, state.time_clock.tm_min, state.time_clock.tm_sec, state.time_clock.tm_mday, state.time_clock.tm_mon, state.time_clock.tm_year);
  Serial.print("Request timestamp to NTP server complete: ");
  Serial.println(ntp_time);

  if (!state.is_ntp_updated) {
    fileSetupLogs();
  }

  String log_string = "NTP Request: " + (String)ntp_time;
  fileWriteWithTimestamp(state.current_files.file_log, log_string, &state.time_clock);

  if (state.is_first_wifi_connect) {
    state.is_first_wifi_connect = false;
  }
  
}


void setupClock() {
  ntpClient.begin();

  updateNTP();
}


void fileSetupLogs() {  
  fileSetup(&state.current_files.file_dat, FILE_SUFFIXES.file_dat, &state.time_clock);
  fileSetup(&state.current_files.file_mdat, FILE_SUFFIXES.file_mdat, &state.time_clock);
  fileSetup(&state.current_files.file_log, FILE_SUFFIXES.file_log, &state.time_clock);

  // Write current rate & thresholds
  String temp_rate_and_timestamp = "Temper'e - Current rate: " + (String)(task_temperature->schedule_ticks/10) + " (secs) - Threshold: " + (String)state.temperature.threshold + " (ºC)";
  fileWriteWithTimestamp(state.current_files.file_log, temp_rate_and_timestamp, &state.time_clock);
  String humi_rate_and_timestamp = "Humidity - Current rate: " + (String)(task_humidity->schedule_ticks/10) + " (secs) - Threshold: " + (String)state.humidity.threshold + " (%)";
  fileWriteWithTimestamp(state.current_files.file_log, humi_rate_and_timestamp, &state.time_clock);
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
      fileSetupLogs();
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
