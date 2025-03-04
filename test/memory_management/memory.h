#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Define the fixed block size in bytes.
#define BLOCK_SIZE 32

// Memory protection flags (for future expansion)
typedef enum {
    MEM_READ  = 0x01,
    MEM_WRITE = 0x02,
    MEM_EXEC  = 0x04
} MemoryFlags;

// Error codes / Memory status
typedef enum {
    MEM_OK = 0,
    MEM_INVALID_POINTER,
    MEM_OUT_OF_HEAP,
    MEM_DOUBLE_FREE,
    MEM_CORRUPTION_DETECTED
} MemoryStatus;

// Memory statistics structure
typedef struct {
    size_t total_heap;
    size_t used_memory;
    size_t peak_usage;
    uint32_t allocation_count;
    uint32_t free_count;
    uint32_t error_count;
} MemoryStats;

// Core Functions

/**
 * Initializes the memory pool.
 * @param heap_size Total size of the memory pool.
 * @param use_guards Boolean flag indicating if guard pages should be used (ignored in this version).
 * @return MemoryStatus code.
 */
MemoryStatus initMemoryPool(size_t heap_size, bool use_guards);

/**
 * Allocates a fixed block of memory (32 bytes).
 * @param flags Memory protection flags (currently ignored).
 * @return Pointer to allocated memory or NULL on failure.
 */
void* allocateMemory(MemoryFlags flags);

/**
 * Frees an allocated memory block.
 * @param ptr Pointer to the memory block to free.
 * @return MemoryStatus code.
 */
MemoryStatus freeMemory(void* ptr);

// Diagnostic Functions

/**
 * Checks the integrity of the heap.
 * @return MemoryStatus code.
 */
MemoryStatus checkHeapIntegrity(void);

/**
 * Returns current memory usage statistics.
 * @return MemoryStats structure.
 */
MemoryStats getMemoryStatistics(void);

/**
 * Dumps a simple memory map to the debug console.
 */
void dumpMemoryMap(void);

// Debugging Utilities
#ifdef MEM_DEBUG
/**
 * Lists current allocations (debug info; not fully implemented).
 */
void listAllocations(void);

/**
 * Sets a breakpoint based on the allocation count (debug utility).
 * @param count Allocation count at which to trigger a breakpoint.
 */
void setAllocBreakpoint(size_t count);
#endif

#endif // MEMORY_H
