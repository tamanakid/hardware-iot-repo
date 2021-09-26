#include "hardware.h"
#include "scheduler.c"


/* Function declarations */
void onReadLightSensor(void);
void onBlinkGreenRGB(void);
void onDetectEvents(void);



schedulerTask *task_light_sensor, *task_green_rgb, *task_detect_events;

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

  task_light_sensor = scheduler_add(4, &onReadLightSensor, true);
  task_green_rgb = scheduler_add(5, &onBlinkGreenRGB, true);
  task_detect_events = scheduler_add(1, &onDetectEvents, true);
}


void loop() {
  scheduler_run();
  delay(100);
}

#define LIGHT_SENSOR_NOISE 20
#define LIGHT_SENSOR_RANGE (1020 - LIGHT_SENSOR_NOISE)
#define LIGHT_SENSOR_STEPS 6
#define LIGHT_SENSOR_THRESHOLD_STEP 3

bool is_overlit = false;
float seconds_overlit = 0;
int light = 0;
int green_rgb = 0;


typedef struct {
  float step_size;
  int pin_led_bar;
} lightStep;
lightStep steps[LIGHT_SENSOR_STEPS] = {
  {0.5, PIN_INT_LED_BAR_0},
  {1.5, PIN_INT_LED_BAR_1},
  {2.5, PIN_INT_LED_BAR_2},
  {3.5, PIN_INT_LED_BAR_3},
  {4.5, PIN_INT_LED_BAR_4},
  {5.5, PIN_INT_LED_BAR_5}
};


void onReadLightSensor() {  
  light = analogRead(PIN_EXT_LIGHT_SENSOR);

  /* Debugging */
  char *buffer = (char*) malloc(40*sizeof(char));

  /* Set the LED bars */
  for (int i = 0; i < LIGHT_SENSOR_STEPS; i++) {
    lightStep stp = steps[i];
    int is_step_active = (light > (LIGHT_SENSOR_RANGE/LIGHT_SENSOR_STEPS)*stp.step_size + LIGHT_SENSOR_NOISE) ? 1 : 0;
    
    digitalWrite(stp.pin_led_bar, is_step_active);

    /* Could be moved to its own task: Check light threshold and overlit count status */
    if (LIGHT_SENSOR_THRESHOLD_STEP == i) {
      seconds_overlit = (is_step_active) ? (seconds_overlit + 0.4) : 0;

      /*
      sprintf(buffer, "light: %d", light);
      Serial.println(buffer);
      Serial.println(seconds_overlit);
      */
    }
  }
}


bool is_buzzer_active = false;

void onDetectEvents() {
  if (seconds_overlit >= 10) {
    if (!is_buzzer_active) {
      is_buzzer_active = true;
      tone(PIN_EXT_BUZZER, BUZZER_NOTE_A5);
    }
  }
  
  int buttonState = digitalRead(PIN_EXT_BUTTON);

  if (is_buzzer_active && buttonState == LOW) {
    is_buzzer_active = false;
    noTone(PIN_EXT_BUZZER);
    // task_detect_events->is_active = false;
  }
}


void onBlinkGreenRGB() {
  if (seconds_overlit >= 5) {
    green_rgb = green_rgb == 0 ? 1 : 0;
  } else {
    green_rgb = 0;
  }
  
  digitalWrite(PIN_INT_LED_GREEN, green_rgb);
}
