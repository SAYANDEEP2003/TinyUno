#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial slaveComm(2, 3);  // RX=2, TX=3

struct Task {
  const char* name;
  int allocated;
  int freed;
};

Task tasks[3] = {
  {"System Monitor", 0, 0},
  {"Data Processor", 0, 0},
  {"Quick Service", 0, 0}
};

void setup() {
  Serial.begin(9600);
  slaveComm.begin(9600);
  Serial.println("MEMORY MANAGER v1.0");
  Serial.println("-------------------");
}

void loop() {
  static int currentTask = 0;
  
  if(millis() % 2000 < 50) {  // Every 2 seconds
    switch(currentTask) {
      case 0: // System Monitor Task
        tasks[0].allocated = 0;
        tasks[0].freed = 0;
        Serial.println("[TASK1] System monitoring completed");
        Serial.println("  -> Released 128B to memory pool");
        break;

      case 1: // Data Processor Task
        slaveComm.write('D');
        while(!slaveComm.available());
        tasks[1].allocated = slaveComm.parseInt();
        tasks[1].freed = slaveComm.parseInt();
        
        Serial.print("[TASK2] Data processed | Allocated: ");
        Serial.print(tasks[1].allocated);
        Serial.print("B | Freed: ");
        Serial.print(tasks[1].freed);
        Serial.println("B");
        break;

      case 2: // Quick Service Task (Smallest)
        slaveComm.write('Q');
        while(!slaveComm.available());
        tasks[2].allocated = slaveComm.parseInt();
        tasks[2].freed = slaveComm.parseInt();
        
        Serial.print("[TASK3] Quick service finished | ");
        Serial.print("Releasing ");
        Serial.print(tasks[2].freed);
        Serial.println("B");
        
        // Distribute freed memory
        tasks[1].allocated += tasks[2].freed/2;
        tasks[0].allocated += tasks[2].freed/2;
        Serial.println("  -> Allocated to TASK1 & TASK2");
        break;
    }

    currentTask = (currentTask + 1) % 3;
    Serial.println("-------------------");
  }
} 