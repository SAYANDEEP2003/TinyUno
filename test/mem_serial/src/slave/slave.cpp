#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial masterComm(2, 3);  // RX=2, TX=3

void setup() {
  masterComm.begin(9600);
}

void memoryOperation(int size) {
  void* ptr = malloc(size);
  delay(100);  // Simulate work
  free(ptr);
}

void loop() {
  if(masterComm.available()) {
    char cmd = masterComm.read();
    
    if(cmd == 'D') {  // Data Processor Task
      void* data = malloc(256);  // Allocate 256B
      delay(500);                // Process data
      free(data);                // Free 256B
      masterComm.print("256,256");  // alloc,freed
    } 
    else if(cmd == 'Q') {  // Quick Service Task
      memoryOperation(64);  // Allocate/free 64B
      masterComm.print("64,64");
    }
    
    masterComm.println();
  }
}