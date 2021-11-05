#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>

#include "hardware.h"
#include "hardware_d1mini.h"
#include "sketch.h"


enum e_buzzer_status {
  SLEEP,
  ON,
  OFF,
};

typedef e_buzzer_status t_buzzer_status;


int seconds = 0;
int buzz_count = 0;
t_buzzer_status buzzer_status = SLEEP;


void setupBuzzer () {
  pinMode(MINID1_PIN_D2, OUTPUT);
}


void taskBuzzer() {
  if (buzzer_status == SLEEP) {
    Serial.println("task:Buzzer> SLEEP->ON");
    
    buzzer_status = ON;
    tone(MINID1_PIN_D2, BUZZER_NOTE_GS5);
  }

  else if (buzzer_status == ON) {
    Serial.println("task:Buzzer> ON->OFF");

    buzz_count++;
    if (buzz_count >= 3) {
      setSchedulerState(ACTION_BUZZER_DEACTIVATE);
      buzzer_status = SLEEP;
      buzz_count = 0;
    } else {
      buzzer_status = OFF;
    }
    noTone(MINID1_PIN_D2);
  }

  else if (buzzer_status == OFF) {
    if (seconds == 1) {
      Serial.println("task:Buzzer> OFF->ON");
      
      seconds = 0;
      buzzer_status = ON;
      tone(MINID1_PIN_D2, BUZZER_NOTE_G5);
    } else {
      seconds++;
    }
  }
}


void resetBuzzer() {
  Serial.println("task:Buzzer> SLEEP");
  
  buzzer_status = SLEEP;
  noTone(MINID1_PIN_D2);
}
