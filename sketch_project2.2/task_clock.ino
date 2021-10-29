#include "sketch.h"


extern t_global_state state;


void setupClock() {
  // Maybe move NTP request here
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
}


void resetClock() {
  
}
