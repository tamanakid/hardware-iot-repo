#include "hardware.h"
#include "scheduler.h"

#include "sketch.h"


/* Global declarations */

t_state state = {
  is_overlit: false,
  is_alarm: false,
  is_sleep: false,
  seconds_overlit: 0,
  seconds_sleep: 0
};

schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler;


/* Setup function */

void setup() {
  analogReference(DEFAULT);
  
  pinMode(PIN_EXT_LIGHT_SENSOR, INPUT);

  pinMode(PIN_EXT_BUZZER, OUTPUT);

  pinMode(PIN_EXT_BUTTON, INPUT_PULLUP);
  
  pinMode(PIN_INT_LED_BAR_0, OUTPUT);
  pinMode(PIN_INT_LED_BAR_1, OUTPUT);
  pinMode(PIN_INT_LED_BAR_2, OUTPUT);
  pinMode(PIN_INT_LED_BAR_3, OUTPUT);
  pinMode(PIN_INT_LED_BAR_4, OUTPUT);
  pinMode(PIN_INT_LED_BAR_5, OUTPUT);

  pinMode(PIN_INT_LED_GREEN, OUTPUT);

  Serial.begin(115200);

  task_timers_tick = scheduler_add(1, &taskTimersTick, true);
  task_light_sensor = scheduler_add(4, &taskLightSensor, true);
  task_green_led_blink = scheduler_add(5, &taskGreenLEDBlink, false);
  task_alarm_handler = scheduler_add(1, &taskAlarmHandler, false);
}



void loop() {
  scheduler_run();
  delay(100);
}
