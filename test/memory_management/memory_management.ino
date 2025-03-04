#include "memory.h"

void setup() {
    Serial.begin(115200);
    delay(2000);  // Allow time for Serial Monitor to initialize

    Serial.println("Starting Fixed Block Memory Management Test...");

    // Step 1: Initialize memory pool (using 1024 bytes for testing)
    MemoryStatus status = initMemoryPool(1024, false);
    if (status != MEM_OK) {
        Serial.println("Memory pool initialization failed!");
        return;
    }

    // Step 2: Allocate several fixed blocks (each 32 bytes)
    Serial.println("Allocating fixed memory blocks...");
    void* block1 = allocateMemory(MEM_READ | MEM_WRITE);
    void* block2 = allocateMemory(MEM_READ | MEM_WRITE);
    void* block3 = allocateMemory(MEM_READ);

    if (!block1 || !block2 || !block3) {
        Serial.println("Memory allocation failed!");
    } else {
        Serial.println("Fixed memory blocks allocated successfully.");
    }

    // Display current memory statistics
    dumpMemoryMap();

    // Step 3: Free one block to simulate fragmentation
    Serial.println("Freeing block1...");
    freeMemory(block1);

    // Display memory statistics after freeing block1
    dumpMemoryMap();

    // Step 4: Allocate another block to test reuse after free
    Serial.println("Allocating another fixed memory block...");
    void* block4 = allocateMemory(MEM_READ | MEM_WRITE);
    if (!block4) {
        Serial.println("Memory allocation for block4 failed!");
    } else {
        Serial.println("Block4 allocated successfully.");
    }

    // Display memory statistics after new allocation
    dumpMemoryMap();

    // Step 5: Free remaining blocks
    Serial.println("Freeing remaining blocks...");
    freeMemory(block2);
    freeMemory(block3);
    freeMemory(block4);

    // Final memory statistics
    dumpMemoryMap();

    // Step 6: Check heap integrity
    Serial.println("Checking heap integrity...");
    checkHeapIntegrity();

    Serial.println("Final Memory Status:");
    dumpMemoryMap();
}

void loop() {
    // Nothing to do in the loop.
}
