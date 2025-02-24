#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>

#define FS_MAGIC 0xAABBCCDD  // Unique identifier for the file system
#define BLOCK_SIZE 512        // Block size in bytes
#define MAX_FILES 32          // Maximum number of files supported
#define FILE_NAME_LENGTH 12   // Fixed-length file names (8.3 format)
#define FILE_TABLE_SECTORS 2  // Number of sectors allocated for the index table
#define DISK_IMAGE "virtual_disk.img"

// Superblock structure (stored at Sector 0)
typedef struct {
    uint32_t magic;        // File system identifier
    uint32_t total_blocks; // Total number of blocks in storage
    uint32_t free_blocks;  // Number of free blocks
    uint32_t file_table_start;  // Start block of file index table
    uint32_t data_start;   // Start block of data storage
} Superblock;

// File metadata structure (stored in File Index Table)
typedef struct {
    char filename[FILE_NAME_LENGTH];  // File name (null-terminated)
    uint32_t start_block; // First block of the file data
    uint32_t size;        // File size in bytes
    uint8_t active;       // 1 = Active file, 0 = Deleted
} FileEntry;

// File System API
int fs_format();   // Format the file system
int fs_mount();    // Mount the file system
void fs_print_info();  // Print file system metadata for debugging

#endif  // FILE_SYSTEM_H