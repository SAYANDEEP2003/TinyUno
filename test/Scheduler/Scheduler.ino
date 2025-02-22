#include "scheduler.h"
#include "led_task.h"
#include "distance_task.h"

void setup() {
  Serial.begin(9600);
  
  // Initialize hardware
  setup_distance_sensor();
  setup_led();
  
  // Initialize scheduler
  scheduler_init();
  
  // Register tasks
  scheduler_register_task("led", led_task_wrapper);
  scheduler_register_task("distance", distance_task_wrapper);
  
  Serial.println("Scheduler ready");
}

void loop() {
  scheduler_handle_command();
  scheduler_run();
  delay(50);
}