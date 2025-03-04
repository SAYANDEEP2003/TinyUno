#include "memory.h"

void setup() {
    Serial.begin(115200);
    delay(2000);  // Allow time for Serial Monitor to initialize

    Serial.println("Starting Memory Management Test...");

    // Step 1: Initialize memory pool (assuming 1024 bytes for testing)
    MemoryStatus status = initMemoryPool(1024, false);
    if (status != MEM_OK) {
        Serial.println("Memory pool initialization failed!");
        return;
    }

    // Step 2: Allocate memory blocks
    Serial.println("Allocating memory blocks...");
    void* block1 = allocateMemory(100, MEM_ALIGNMENT, MEM_READ | MEM_WRITE);
    void* block2 = allocateMemory(200, MEM_ALIGNMENT, MEM_READ | MEM_WRITE);
    void* block3 = allocateMemory(50, MEM_ALIGNMENT, MEM_READ);

    if (!block1 || !block2 || !block3) {
        Serial.println("Memory allocation failed!");
    } else {
        Serial.println("Memory blocks allocated successfully.");
    }

    // Print current memory statistics
    dumpMemoryMap();

    // Step 3: Free some memory blocks to simulate fragmentation
    Serial.println("Freeing memory blocks block1 and block2...");
    if (block1) freeMemory(block1, 100);
    if (block2) freeMemory(block2, 200);

    // Print updated memory statistics
    dumpMemoryMap();

    // Step 4: Allocate a new block to test reuse after fragmentation
    Serial.println("Allocating new memory block (150 bytes) to test fragmentation handling...");
    void* block4 = allocateMemory(150, MEM_ALIGNMENT, MEM_READ | MEM_WRITE);
    if (!block4) {
        Serial.println("Memory allocation for block4 failed!");
    } else {
        Serial.println("Memory block block4 allocated successfully.");
    }

    // Print memory statistics after fragmentation test
    dumpMemoryMap();

    // Step 5: Free remaining blocks
    Serial.println("Freeing remaining memory blocks...");
    if (block3) freeMemory(block3, 50);
    if (block4) freeMemory(block4, 150);

    // Final memory statistics
    dumpMemoryMap();

    // Step 6: Check heap integrity
    Serial.println("Checking heap integrity...");
    checkHeapIntegrity();

    // Final Memory Status
    Serial.println("Final Memory Status:");
    dumpMemoryMap();
}

void loop() {
    // Nothing to do in the loop
}
