#include "scheduler.h"
#include "led_task.h"
#include "distance_task.h"

void setup() {
    Serial.begin(9600);
    setup_led();
    setup_distance_sensor();

    // Register tasks
    scheduler_register_task("led", led_task_wrapper);
    scheduler_register_task("distance", distance_task_wrapper);

    // Add tasks to alternate every 10s, with 5s duration
    // scheduler_add_task("led", 10000, 5000, 2); // Higher priority
    // scheduler_add_task("distance", 10000, 5000, 1); // Lower priority
}

void loop() {
    scheduler_run();
    scheduler_handle_command();
}