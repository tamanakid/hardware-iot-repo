#include <NTPClient.h>

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

  state.timestamp.hours = ntpClient.getHours();
  state.timestamp.minutes = ntpClient.getMinutes();
  state.timestamp.seconds = ntpClient.getSeconds();
}


void taskClock() {
  state.timestamp.seconds = state.timestamp.seconds + 1;
  
  if (state.timestamp.seconds == 60) {
    state.timestamp.seconds = 0;
    state.timestamp.minutes = state.timestamp.minutes + 1;
  }

  if (state.timestamp.minutes == 60) {
    state.timestamp.minutes = 0;
    state.timestamp.hours = state.timestamp.hours + 1;
  }

  if (state.timestamp.hours == 24) {
    state.timestamp.hours = 0;
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
