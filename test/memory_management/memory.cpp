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

    // Manually set the heap start address
    mem.heap_start = (void*)970; // Use the observed address
    if (mem.heap_start == NULL) {
        return MEM_OUT_OF_HEAP;
    }

    // Initialize heap boundaries
    mem.heap_size = heap_size;
    mem.free_list = (AllocHeader*)mem.heap_start;
    mem.free_list->size = heap_size;
    mem.free_list->next = NULL;

    // Initialize memory statistics
    mem.stats.total_heap = heap_size;
    mem.stats.used_memory = 0;
    mem.stats.peak_usage = 0;
    mem.stats.allocation_count = 0;
    mem.stats.free_count = 0;
    mem.stats.error_count = 0;

    return MEM_OK;
}

void* allocateMemory(size_t size, size_t alignment, MemoryFlags flags) {
    if (size == 0) {
        return NULL;
    }

    // Calculate total size including header and alignment padding
    size_t total_size = size + sizeof(AllocHeader);
    size_t alignment_padding = 0;

    // Align the memory block if necessary
    if (alignment > 0) {
        alignment_padding = (alignment - (sizeof(AllocHeader) % alignment)) % alignment;
        total_size += alignment_padding;
    }

    AllocHeader* current = mem.free_list;
    AllocHeader* prev = NULL;

    while (current != NULL) {
        // Check if the current block is large enough
        if (current->size >= total_size) {
            // Calculate the address of the allocated memory
            uintptr_t aligned_addr = (uintptr_t)current + sizeof(AllocHeader) + alignment_padding;

            // Split the block if there's enough space left
            if (current->size > total_size) {
                AllocHeader* new_block = (AllocHeader*)((uint8_t*)current + total_size);
                new_block->size = current->size - total_size;
                new_block->next = current->next;
                current->size = total_size;
                current->next = new_block;
            } else {
                // Remove the block from the free list
                if (prev != NULL) {
                    prev->next = current->next;
                } else {
                    mem.free_list = current->next;
                }
            }

            // Initialize the allocated block
            current->magic = 0xDEADBEEF;
            current->flags = flags;

            // Update memory statistics
            mem.stats.used_memory += size;
            mem.stats.allocation_count++;
            if (mem.stats.used_memory > mem.stats.peak_usage) {
                mem.stats.peak_usage = mem.stats.used_memory;
            }

            // Return the address of the allocated memory
            return (void*)aligned_addr;
        }

        // Move to the next block
        prev = current;
        current = current->next;
    }

    // Out of memory
    return NULL;
}

MemoryStatus freeMemory(void* ptr, size_t size) {
    if (ptr == NULL) {
        return MEM_INVALID_POINTER;
    }

    // Get the header of the block being freed
    AllocHeader* header = (AllocHeader*)((uint8_t*)ptr - sizeof(AllocHeader));
    if (header->magic != 0xDEADBEEF) {
        return MEM_CORRUPTION_DETECTED;
    }

    // Find the correct position in the free list to insert the block
    AllocHeader* current = mem.free_list;
    AllocHeader* prev = NULL;
    while (current != NULL && current < header) {
        prev = current;
        current = current->next;
    }

    // Merge with the previous block if adjacent
    if (prev != NULL && (uint8_t*)prev + prev->size == (uint8_t*)header) {
        prev->size += header->size;
        header = prev;
    } else {
        // Insert the block into the free list
        header->next = current;
        if (prev != NULL) {
            prev->next = header;
        } else {
            mem.free_list = header;
        }
    }

    // Merge with the next block if adjacent
    if (current != NULL && (uint8_t*)header + header->size == (uint8_t*)current) {
        header->size += current->size;
        header->next = current->next;
    }

    // Update memory statistics
    mem.stats.used_memory -= size;
    mem.stats.free_count++;

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
    Serial.println("Memory Map:");
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
