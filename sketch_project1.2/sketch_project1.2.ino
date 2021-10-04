#include "hardware.h"
#include "scheduler.h"

#include "sketch.h"


/* Global declarations */

t_global_state state = {
  is_overlit: false,
  is_wrist_active: false,
  alarm_state: ALARM_OFF,
  seconds_overlit: 0,
  seconds_sleep: 0
};

schedulerTask *task_timers_tick, *task_light_sensor, *task_green_led_blink, *task_alarm_handler, *task_wrist_sensor;


/* Setup function */

void setup() {
  analogReference(DEFAULT);

  setupLightSensor();
  setupGreenLEDBlink();
  setupAlarmHandler();
  setupWristSensor();

  Serial.begin(115200);

  task_timers_tick = scheduler_add(1, &taskTimersTick, true);
  task_light_sensor = scheduler_add(4, &taskLightSensor, false);
  task_green_led_blink = scheduler_add(5, &taskGreenLEDBlink, false);
  task_alarm_handler = scheduler_add(1, &taskAlarmHandler, false);
  task_wrist_sensor = scheduler_add(2, &taskWristSensor, true);
}



void loop() {
  scheduler_run();
  delay(100);
}



void setSchedulerState (t_reducer_action action) {
  static t_reducer_action last_action = RDX_WRIST_DEACTIVATE;

  if (last_action == action) {
    return;
  }
  
  switch (action) {
    case RDX_WRIST_ACTIVATE:
      Serial.println(">> State Change: Wrist Activated");
      state.is_wrist_active = true;
      scheduler_activate(task_light_sensor);
      break;

    case RDX_WRIST_DEACTIVATE:
      Serial.println(">> State Change: Wrist Deactivated");
      state.is_wrist_active = false;
      scheduler_deactivate(task_light_sensor);
      onResetLightSensor();
      break;
      
    case RDX_GREEN_LED_ON:
      Serial.println(">> State Change: Alarm on Green LED blinking");
      scheduler_activate(task_green_led_blink);
      break;
      
    case RDX_ALARM_ON:
      Serial.println(">> State Change: Alarm Activated");
      state.alarm_state = ALARM_ON;
      scheduler_activate(task_alarm_handler);
      scheduler_deactivate(task_timers_tick);
      break;
      
    case RDX_ALARM_SLEEP:
      Serial.println(">> State Change: Alarm on Sleep Mode");
      state.alarm_state = ALARM_SLEEP;
      scheduler_activate(task_timers_tick);
      scheduler_deactivate(task_light_sensor);
      scheduler_deactivate(task_green_led_blink);
      scheduler_deactivate(task_alarm_handler);
      onResetLightSensor();
      onResetGreenLEDBlink();
      onResetAlarmHandler();
      break;

    case RDX_ALARM_RESET:
      Serial.println(">> State Change: Alarm on Standby");
      state.alarm_state = ALARM_OFF;
      scheduler_activate(task_light_sensor);
      scheduler_deactivate(task_green_led_blink);
      onResetGreenLEDBlink();
      break;
  }
  
  last_action = action;
}
