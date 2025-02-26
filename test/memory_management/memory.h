#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>     // For size_t
#include <stdint.h>     // For uint32_t etc
#include "common.h"     // For kernel-specific types

// Memory alignment requirement (architecture-specific)
#define MEM_ALIGNMENT 8

// Memory protection flags
typedef enum {
    MEM_READ = 0x01,
    MEM_WRITE = 0x02,
    MEM_EXEC = 0x04
} MemoryFlags;

// Error codes
typedef enum {
    MEM_OK = 0,
    MEM_INVALID_SIZE,
    MEM_OUT_OF_HEAP,
    MEM_DOUBLE_FREE,
    MEM_CORRUPTION_DETECTED,
    MEM_INVALID_POINTER
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

// Initializes memory pool with optional guard pages
MemoryStatus initMemoryPool(size_t heap_size, bool use_guards);

// Enhanced allocation with alignment and protection flags
void* allocateMemory(size_t size, size_t alignment, MemoryFlags flags);

// Safe memory free with integrity checks
MemoryStatus freeMemory(void* ptr, size_t size);

// Memory protection management
MemoryStatus protectMemoryRegion(void* addr, size_t size, MemoryFlags flags);

// Diagnostic functions
MemoryStatus checkHeapIntegrity(void);
MemoryStats getMemoryStatistics(void);
void dumpMemoryMap(void);

// Debugging utilities
#ifdef MEM_DEBUG
void listAllocations(void);
void setAllocBreakpoint(size_t count);
#endif

#endif // MEMORY_H
