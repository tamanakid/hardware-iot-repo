#include "sketch.h"


extern t_global_state state;

void setupWristSensor () {
  pinMode(PIN_ACC_X, INPUT);
  pinMode(PIN_ACC_Y, INPUT);
  pinMode(PIN_ACC_Z, INPUT);
}


void taskWristSensor () {
  static int x_reg[4], y_reg[4], z_reg[4];
  int x_diff[3], y_diff[3], z_diff[3];

  /* Shift registers */
  for (int i = 3; i > 0; i--) {
    x_reg[i] = x_reg[i-1];
    y_reg[i] = y_reg[i-1];
    z_reg[i] = z_reg[i-1];
  }
  
  x_reg[0] = analogRead(PIN_ACC_X);
  y_reg[0] = analogRead(PIN_ACC_Y);
  z_reg[0] = analogRead(PIN_ACC_Z);

  for (int j = 0; j < 3; j++) {    
    x_diff[j] = x_reg[0] - x_reg[j + 1];
    y_diff[j] = y_reg[0] - y_reg[j + 1];
    z_diff[j] = z_reg[0] - z_reg[j + 1];
    // debug(j, x_diff[j], y_diff[j], z_diff[j]);

    if (y_diff[j] < -80) { //  && z_diff[j] > 50
      Serial.println("Wrist movement detected to turn ON...");
      if (state.alarm_state != ALARM_SLEEP) {
        setSchedulerState(ACTION_WRIST_ACTIVATE);
      }
      break;
    }

    if (y_diff[j] > 80) { // && z_diff[j] < -50
      Serial.println("Wrist movement detected to turn OFF...");
      if (state.alarm_state != ALARM_SLEEP) {
        setSchedulerState(ACTION_WRIST_DEACTIVATE);
      }
      break;
    }
    
  }
}


void debug(int j, int x_diff, int y_diff, int z_diff) {
  Serial.print(200*(j+1));
  Serial.print(" ms diff");
  Serial.print(" - x: ");
  Serial.print(x_diff);
  Serial.print(" - y: ");
  Serial.print(y_diff);
  Serial.print(" - z: ");
  Serial.println(z_diff);
}
