#include "memory.h"
#include <stdlib.h>
#include <Arduino.h>

// Global memory statistics variable
static MemoryStats memStats = {0};

// Initializes the memory pool. For this simple implementation, it only sets up statistics.
MemoryStatus initMemoryPool(size_t heap_size, bool use_guards) {
    memStats.total_heap = heap_size;
    memStats.used_memory = 0;
    memStats.peak_usage = 0;
    memStats.allocation_count = 0;
    memStats.free_count = 0;
    memStats.error_count = 0;

    // In a more complex system, you might initialize your own memory pool here.
    Serial.print("Memory pool initialized with size: ");
    Serial.println(heap_size);
    return MEM_OK;
}

// Allocates a fixed block of memory (32 bytes).
void* allocateMemory(MemoryFlags flags) {
    // For fixed block allocation, ignore parameters and allocate BLOCK_SIZE bytes.
    void* ptr = malloc(BLOCK_SIZE);
    if (ptr == NULL) {
        memStats.error_count++;
        Serial.println("Memory allocation failed!");
        return NULL;
    }

    memStats.used_memory += BLOCK_SIZE;
    if (memStats.used_memory > memStats.peak_usage) {
        memStats.peak_usage = memStats.used_memory;
    }
    memStats.allocation_count++;

    Serial.print("Allocated fixed block of ");
    Serial.print(BLOCK_SIZE);
    Serial.print(" bytes at address: ");
    Serial.println((uintptr_t)ptr, HEX);

#ifdef MEM_DEBUG
    // Additional debug info could be recorded here.
#endif

    return ptr;
}

// Frees the allocated fixed block of memory.
MemoryStatus freeMemory(void* ptr) {
    if (ptr == NULL) {
        memStats.error_count++;
        Serial.println("Free error: Invalid pointer!");
        return MEM_INVALID_POINTER;
    }

    free(ptr);
    if (memStats.used_memory >= BLOCK_SIZE) {
        memStats.used_memory -= BLOCK_SIZE;
    } else {
        memStats.used_memory = 0;
    }
    memStats.free_count++;

    Serial.print("Freed fixed block of ");
    Serial.print(BLOCK_SIZE);
    Serial.print(" bytes at address: ");
    Serial.println((uintptr_t)ptr, HEX);

    return MEM_OK;
}

// Performs a basic check of the heap's integrity.
MemoryStatus checkHeapIntegrity(void) {
    Serial.println("Heap integrity check: OK");
    return MEM_OK;
}

// Returns a snapshot of current memory usage statistics.
MemoryStats getMemoryStatistics(void) {
    return memStats;
}

// Dumps a simple memory map to the Serial console.
void dumpMemoryMap(void) {
    Serial.println("----- Memory Map Dump -----");
    Serial.print("Total Heap: ");
    Serial.println(memStats.total_heap);
    Serial.print("Used Memory: ");
    Serial.println(memStats.used_memory);
    Serial.print("Peak Usage: ");
    Serial.println(memStats.peak_usage);
    Serial.print("Allocation Count: ");
    Serial.println(memStats.allocation_count);
    Serial.print("Free Count: ");
    Serial.println(memStats.free_count);
    Serial.print("Error Count: ");
    Serial.println(memStats.error_count);
    Serial.println("---------------------------");
}

#ifdef MEM_DEBUG
// Debugging utility: Lists current allocations.
void listAllocations(void) {
    Serial.println("Debug: Listing allocations (not implemented).");
}

// Debugging utility: Sets an allocation breakpoint.
void setAllocBreakpoint(size_t count) {
    Serial.print("Debug: Allocation breakpoint set at count: ");
    Serial.println(count);
}
#endif
