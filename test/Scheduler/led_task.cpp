#include "led_task.h"
#include <Arduino.h> // Add this line

void setup_led() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void led_task_wrapper() {
  static unsigned long last_toggle = 0;
  static bool led_state = LOW;
  
  if (millis() - last_toggle >= 1000) {
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
    last_toggle = millis();
  }
}