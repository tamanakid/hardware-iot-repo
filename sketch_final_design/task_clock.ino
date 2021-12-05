#include <NTPClient.h>
#include "time.h"

#include "sketch.h"


extern t_global_state state;

extern WiFiUDP UdpInstance;

const int utcOffsetInSeconds = 3600;
NTPClient ntpClient(UdpInstance, "1.es.pool.ntp.org", utcOffsetInSeconds);


void setupClock() {
  ntpClient.begin();

  Serial.println("Requesting timestamp to NTP server...");
  ntpClient.update();
  Serial.println("Request timestamp to NTP server complete");

  time_t timestamp;
  struct tm time_struct;
  
  timestamp = ntpClient.getEpochTime();
  time_struct = *localtime(&timestamp);
   
  state.time_clock.year = time_struct.tm_year + 1900;
  state.time_clock.month = time_struct.tm_mon + 1;
  state.time_clock.day = time_struct.tm_mday;

  state.time_clock.hour = time_struct.tm_hour;
  state.time_clock.minute = time_struct.tm_min;
  state.time_clock.second = time_struct.tm_sec;
  // seg=fecha_hora.tm_hour*3600+fecha_hora.tm_min*60+fecha_hora.tm_sec+1;
}


void taskClock() {
  state.time_clock.second = state.time_clock.second + 1;
  
  if (state.time_clock.second == 60) {
    state.time_clock.second = 0;
    state.time_clock.minute = state.time_clock.minute + 1;
    // if state.timestamp.minutes % 5 == 0
    temperatureMeanReadAndReset();
    humidityMeanReadAndReset();
  }

  if (state.time_clock.minute == 60) {
    state.time_clock.minute = 0;
    state.time_clock.hour = state.time_clock.hour + 1;
  }

  if (state.time_clock.hour == 24) {
    state.time_clock.hour = 0;
    // call ntp request again
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
