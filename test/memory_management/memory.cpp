#include "memory.h"
#include <Arduino.h>
#include <avr/pgmspace.h>  // For PROGMEM if needed

// Metadata structure for each allocation block
typedef struct AllocHeader {
    size_t size;
    uint32_t magic;
    #ifdef MEM_GUARDS
    uint32_t front_guard;
    #endif
    uint8_t flags;
    struct AllocHeader* next;
} AllocHeader;

// Global memory management state
typedef struct {
    void* heap_start;
    size_t heap_size;
    AllocHeader* free_list;
    MemoryStats stats;
    #ifdef MEM_THREADSAFE
    Mutex mem_mutex;  // Assuming you have mutex implementation
    #endif
} MemoryManager;

static MemoryManager mem;

MemoryStatus initMemoryPool(size_t heap_size, bool use_guards) {
    // Check for invalid heap size
    if (heap_size == 0) {
        return MEM_INVALID_SIZE;
    }

    // Initialize heap boundaries
    mem.heap_start = malloc(heap_size);
    if (mem.heap_start == NULL) {
        return MEM_OUT_OF_HEAP;
    }

    // Rest of the function remains the same
    mem.heap_size = heap_size;
    mem.free_list = (AllocHeader*)mem.heap_start;
    mem.free_list->size = heap_size;
    mem.free_list->next = NULL;
    mem.stats.total_heap = heap_size;
    mem.stats.used_memory = 0;
    mem.stats.peak_usage = 0;
    mem.stats.allocation_count = 0;
    mem.stats.free_count = 0;
    mem.stats.error_count = 0;

    // Configure guard pages if enabled
    if (use_guards) {
        // Placeholder for guard page implementation
        //yet to be implemented
    }

    return MEM_OK;
}

void* allocateMemory(size_t size, size_t alignment, MemoryFlags flags) {
    size_t total_size = size + sizeof(AllocHeader);
    AllocHeader* current = mem.free_list;
    AllocHeader* prev = NULL;

    Serial.print("Allocating memory of size: ");
    Serial.println(size);

    while (current != NULL) {
        Serial.print("Checking block at address: ");
        Serial.print((uintptr_t)current);
        Serial.print(" with size: ");
        Serial.println(current->size);

        // Align the memory block if necessary
        uintptr_t aligned_addr = (uintptr_t)((uint8_t*)current + sizeof(AllocHeader));
        size_t offset = 0;
        if (alignment > 0) {
            offset = (alignment - (aligned_addr % alignment)) % alignment;
            aligned_addr += offset;
            total_size += offset;
        }

        if (current->size >= total_size) {
            if (current->size > total_size) {
                AllocHeader* new_block = (AllocHeader*)((uint8_t*)current + total_size);
                new_block->size = current->size - total_size;
                new_block->next = current->next;
                current->size = total_size;
                current->next = new_block;
            } else {
                if (prev != NULL) {
                    prev->next = current->next;
                } else {
                    mem.free_list = current->next;
                }
            }

            current->magic = 0xDEADBEEF;
            current->flags = flags;
            mem.stats.used_memory += size;
            mem.stats.allocation_count++;
            if (mem.stats.used_memory > mem.stats.peak_usage) {
                mem.stats.peak_usage = mem.stats.used_memory;
            }

            uintptr_t addr = aligned_addr;
            Serial.print("Memory allocated at address: ");
            Serial.println(addr);
            return (void*)addr;
        }
        prev = current;
        current = current->next;
    }

    Serial.println("Out of memory!");
    return NULL;
}

MemoryStatus freeMemory(void* ptr, size_t size) {
    if (ptr == NULL) {
        return MEM_INVALID_POINTER;
    }

    AllocHeader* header = (AllocHeader*)((uint8_t*)ptr - sizeof(AllocHeader));
    if (header->magic != 0xDEADBEEF) {
        return MEM_CORRUPTION_DETECTED;
    }

    AllocHeader* current = mem.free_list;
    AllocHeader* prev = NULL;
    while (current != NULL && current < header) {
        prev = current;
        current = current->next;
    }

    if (prev != NULL && (uint8_t*)prev + prev->size == (uint8_t*)header) {
        prev->size += header->size;
        header = prev;
    }

    if (current != NULL && (uint8_t*)header + header->size == (uint8_t*)current) {
        header->size += current->size;
        header->next = current->next;
    } else {
        header->next = current;
    }

    if (prev == NULL) {
        mem.free_list = header;
    } else {
        prev->next = header;
    }

    mem.stats.used_memory -= size;
    mem.stats.free_count++;

    Serial.print("Freed memory at address: ");
    Serial.println((uintptr_t)ptr);
    return MEM_OK;
}

MemoryStatus checkHeapIntegrity(void) {
    AllocHeader* current = mem.free_list;
    while (current != NULL) {
        if (current->magic != 0xDEADBEEF) {
            return MEM_CORRUPTION_DETECTED;
        }
        current = current->next;
    }
    return MEM_OK;
}

MemoryStats getMemoryStatistics(void) {
    return mem.stats;
}

void dumpMemoryMap(void) {
    AllocHeader* current = mem.free_list;
    while (current != NULL) {
        Serial.print("Block at: ");
        Serial.print((uintptr_t)current);
        Serial.print(" Size: ");
        Serial.println(current->size);
        current = current->next;
    }
}

#ifdef MEM_DEBUG
void listAllocations(void) {
    AllocHeader* current = mem.free_list;
    while (current != NULL) {
        Serial.print("Allocation at: ");
        Serial.print((uintptr_t)current);
        Serial.print(" Size: ");
        Serial.println(current->size);
        current = current->next;
    }
}

void setAllocBreakpoint(size_t count) {
    // Implement allocation breakpoint
}
#endif
