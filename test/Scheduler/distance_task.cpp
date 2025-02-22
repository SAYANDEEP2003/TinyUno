#include "distance_task.h"
#include <Arduino.h> // Add this line

const int trigPin = 9;
const int echoPin = 10;

void setup_distance_sensor() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void distance_task_wrapper() {
  static unsigned long last_measure = 0;
  
  if (millis() - last_measure >= 200) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;
    
    Serial.print("Distance: ");
    Serial.println(distance);
    
    last_measure = millis();
  }
}