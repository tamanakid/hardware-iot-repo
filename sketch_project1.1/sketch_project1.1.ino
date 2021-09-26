#include "hardware.h"
#include "scheduler.c"




void setup() {
  analogReference(DEFAULT);
  
  pinMode(PIN_EXT_LIGHT_SENSOR, INPUT);
  
  pinMode(PIN_INT_LED_BAR_0, OUTPUT);
  pinMode(PIN_INT_LED_BAR_1, OUTPUT);
  pinMode(PIN_INT_LED_BAR_2, OUTPUT);
  pinMode(PIN_INT_LED_BAR_3, OUTPUT);
  pinMode(PIN_INT_LED_BAR_4, OUTPUT);
  pinMode(PIN_INT_LED_BAR_5, OUTPUT);

  pinMode(PIN_INT_LED_GREEN, OUTPUT);

  Serial.begin(9600);

  schedulerTask *task_light_sensor = scheduler_add(4, &onReadLightSensor, true);
  schedulerTask *task_green_rgb = scheduler_add(5, &onBlinkGreenRGB, true);
}

int light = 0;


#define LIGHT_SENSOR_NOISE 20
#define LIGHT_SENSOR_RANGE (1020 - LIGHT_SENSOR_NOISE)
#define LIGHT_SENSOR_STEPS 6
#define LIGHT_SENSOR_THRESHOLD_STEP 4;


void loop() {
  scheduler_run();
  delay(100);
}


void onReadLightSensor() {  
  light = analogRead(PIN_EXT_LIGHT_SENSOR);

  int writeBar0 = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*0.5 + LIGHT_SENSOR_NOISE) ? 1 : 0;
  int writeBar1 = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*1.5 + LIGHT_SENSOR_NOISE) ? 1 : 0;
  int writeBar2 = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*2.5 + LIGHT_SENSOR_NOISE) ? 1 : 0;
  int writeBar3 = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*3.5 + LIGHT_SENSOR_NOISE) ? 1 : 0;
  int writeBar4 = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*4.5 + LIGHT_SENSOR_NOISE) ? 1 : 0;
  int writeBar5 = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*5.5 + LIGHT_SENSOR_NOISE) ? 1 : 0;

  digitalWrite(PIN_INT_LED_BAR_0, writeBar0);
  digitalWrite(PIN_INT_LED_BAR_1, writeBar1);
  digitalWrite(PIN_INT_LED_BAR_2, writeBar2);
  digitalWrite(PIN_INT_LED_BAR_3, writeBar3);
  digitalWrite(PIN_INT_LED_BAR_4, writeBar4);
  digitalWrite(PIN_INT_LED_BAR_5, writeBar5);
}



int green_rgb = 0;

void onBlinkGreenRGB() {
  green_rgb = green_rgb == 0 ? 1 : 0;
  digitalWrite(PIN_INT_LED_GREEN, green_rgb);
}
