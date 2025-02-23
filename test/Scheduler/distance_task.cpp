#include "distance_task.h"
#include <Arduino.h>

// Example for an ultrasonic sensor (HC-SR04)
const int trigPin = 9;
const int echoPin = 10;

void setup_distance_sensor() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void distance_task_wrapper() {
    // Measure distance (example logic)
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;

    Serial.print("Distance: ");
    Serial.println(distance);
}