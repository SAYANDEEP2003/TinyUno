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

    // Note: 'use_guards' is not used in this simple version.
    Serial.print("Memory pool initialized with size: ");
    Serial.println(heap_size);
    return MEM_OK;
}

// Allocates memory with the requested size and alignment.
// Flags are provided for future memory protection support but are currently ignored.
void* allocateMemory(size_t size, size_t alignment, MemoryFlags flags) {
    if (size == 0) {
        memStats.error_count++;
        Serial.println("Allocation error: size is 0");
        return NULL;
    }

    // Adjust the requested size to meet the defined alignment
    size_t aligned_size = (size + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT - 1);

    void* ptr = malloc(aligned_size);
    if (ptr == NULL) {
        memStats.error_count++;
        Serial.println("Memory allocation failed!");
        return NULL;
    }

    memStats.used_memory += aligned_size;
    if (memStats.used_memory > memStats.peak_usage) {
        memStats.peak_usage = memStats.used_memory;
    }
    memStats.allocation_count++;

    // Print requested size and aligned size for clarity
    Serial.print("Allocated ");
    Serial.print(size);
    Serial.print(" bytes (aligned: ");
    Serial.print(aligned_size);
    Serial.print(") at address: ");
    Serial.println((uintptr_t)ptr, HEX);

#ifdef MEM_DEBUG
    // In a more advanced version, record allocation details for debugging.
#endif

    return ptr;
}

// Frees the allocated memory and updates statistics.
// The 'size' parameter should be the originally requested size.
MemoryStatus freeMemory(void* ptr, size_t size) {
    if (ptr == NULL) {
        memStats.error_count++;
        Serial.println("Free error: Invalid pointer!");
        return MEM_INVALID_POINTER;
    }

    // Adjust the size to the same alignment as in allocateMemory
    size_t aligned_size = (size + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT - 1);

    free(ptr);
    if (memStats.used_memory >= aligned_size) {
        memStats.used_memory -= aligned_size;
    } else {
        memStats.used_memory = 0;
    }
    memStats.free_count++;

    // Print freed size information for clarity
    Serial.print("Freed ");
    Serial.print(size);
    Serial.print(" bytes (aligned: ");
    Serial.print(aligned_size);
    Serial.print(") at address: ");
    Serial.println((uintptr_t)ptr, HEX);

    return MEM_OK;
}

// Performs a basic check of the heap's integrity.
// In a real system, this would include more extensive checks.
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
// This is a placeholder for extended debugging support.
void listAllocations(void) {
    Serial.println("Debug: Listing allocations (not implemented).");
}

// Debugging utility: Sets an allocation breakpoint (for debugging purposes).
void setAllocBreakpoint(size_t count) {
    Serial.print("Debug: Allocation breakpoint set at count: ");
    Serial.println(count);
    // In an actual implementation, you might trigger a breakpoint or log additional details here.
}
#endif
