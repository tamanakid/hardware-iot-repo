#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "hardware_d1mini.h"
#include "sketch.h"


enum e_buzzer_status {
  SLEEP,
  ON,
  OFF,
};

typedef e_buzzer_status t_buzzer_status;


int seconds = 0;
t_buzzer_status buzzer_status = SLEEP;


void setupBuzzer () {
  pinMode(MINID1_PIN_D3, OUTPUT);
  digitalWrite(MINID1_PIN_D3, LOW);
}


void taskBuzzer() {
  if (buzzer_status == SLEEP) {
    // Serial.println("task:Buzzer> SLEEP->ON");
    
    buzzer_status = ON;
    digitalWrite(MINID1_PIN_D3, HIGH);
  }

  else if (buzzer_status == ON) {
    // Serial.println("task:Buzzer> ON->OFF");
    
    buzzer_status = OFF;
    digitalWrite(MINID1_PIN_D3, LOW);
  }

  else if (buzzer_status == OFF) {
    if (seconds == 1) {
      // Serial.println("task:Buzzer> OFF->ON");
      
      seconds = 0;
      buzzer_status = ON;
      digitalWrite(MINID1_PIN_D3, HIGH);
    } else {
      seconds++;
    }
  }
}


void resetBuzzer() {
  // Serial.println("task:Buzzer> SLEEP");
  
  buzzer_status = SLEEP;
  digitalWrite(MINID1_PIN_D3, LOW);
}
