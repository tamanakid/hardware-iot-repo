#include "hardware.h"

#define PIN_ACC_X A7
#define PIN_ACC_Y A8
#define PIN_ACC_Z A9


void setup() {
  analogReference(DEFAULT);

  pinMode(PIN_ACC_X, INPUT);
  pinMode(PIN_ACC_Y, INPUT);
  pinMode(PIN_ACC_Z, INPUT);

  Serial.begin(115200);

  delay(2000);
}



void loop() {
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

    if (x_diff[j] > 10 || y_diff[j] > 10 || z_diff[j] > 10) {
      Serial.print(200*(j+1));
      Serial.print(" ms diff");
      Serial.print(" - x: ");
      Serial.print(x_diff[j]);
      Serial.print(" - y: ");
      Serial.print(y_diff[j]);
      Serial.print(" - z: ");
      Serial.println(z_diff[j]);
    }
    

    if (y_diff[j] < -50) { // && z_diff[j] > 50
      Serial.print("ON - Wrist movement detected");
      Serial.print(" - Latency: ");
      Serial.print(200*(j+1));
      Serial.print(" - X: ");
      Serial.print(x_diff[j]);
      Serial.print(" - Y: ");
      Serial.print(y_diff[j]);
      Serial.print(" - Z: ");
      Serial.println(z_diff[j]);
      break;
    }

    if (y_diff[j] > 50) { // && z_diff[j] < -50
      Serial.print("OFF - Wrist movement detected");
      Serial.print(" - Latency: ");
      Serial.print(200*(j+1));
      Serial.print(" - X: ");
      Serial.print(x_diff[j]);
      Serial.print(" - Y: ");
      Serial.print(y_diff[j]);
      Serial.print(" - Z: ");
      Serial.println(z_diff[j]);
      break;
    }
    
  }

  delay(200);
}
