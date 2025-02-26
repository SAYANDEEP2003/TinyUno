#include "memory.h"

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for serial port to connect
    }

    // Initialize memory pool
    MemoryStatus status = initMemoryPool(1024, false);
    if (status != MEM_OK) {
        Serial.println("Memory pool initialization failed!");
        return;
    }

    // Allocate memory
    void* ptr1 = allocateMemory(128, MEM_ALIGNMENT, MEM_READ | MEM_WRITE);
    if (ptr1 == NULL) {
        Serial.println("Memory allocation failed!");
        return;
    }

    // Allocate more memory
    void* ptr2 = allocateMemory(256, MEM_ALIGNMENT, MEM_READ | MEM_WRITE);
    if (ptr2 == NULL) {
        Serial.println("Memory allocation failed!");
        return;
    }

    // Free memory
    status = freeMemory(ptr1, 128);
    if (status != MEM_OK) {
        Serial.println("Memory free failed!");
        return;
    }

    // Check heap integrity
    status = checkHeapIntegrity();
    if (status != MEM_OK) {
        Serial.println("Heap integrity check failed!");
        return;
    }
    Serial.println("Heap integrity check passed!");

    // Get memory statistics
    MemoryStats stats = getMemoryStatistics();
    Serial.print("Total heap: ");
    Serial.println(stats.total_heap);
    Serial.print("Used memory: ");
    Serial.println(stats.used_memory);
    Serial.print("Peak usage: ");
    Serial.println(stats.peak_usage);
    Serial.print("Allocation count: ");
    Serial.println(stats.allocation_count);
    Serial.print("Free count: ");
    Serial.println(stats.free_count);
    Serial.print("Error count: ");
    Serial.println(stats.error_count);

    // Dump memory map
    dumpMemoryMap();
}

void loop() {
    // Nothing to do here
}
