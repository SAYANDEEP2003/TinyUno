#include "file_system.h"
#include <stdio.h>
#include <string.h>

#define TOTAL_STORAGE_BLOCKS 1024  // Total blocks in storage

// Simulated storage (Replace with actual SD card/EERPOM access)
uint8_t storage[TOTAL_STORAGE_BLOCKS][BLOCK_SIZE];
FILE* file;

// Global file system structures
Superblock superblock;
FileEntry file_table[MAX_FILES];

// Function to write the storage array to the disk image file
int write_disk_image() {
    file = fopen(DISK_IMAGE, "wb");
    if (!file) {
        printf("Failed to create disk image!\n");
        return -1;
    }

    // Write the entire storage array to the file
    fwrite(storage, BLOCK_SIZE, TOTAL_STORAGE_BLOCKS, file);
    fclose(file);
    printf("Disk image created: %s\n", DISK_IMAGE);
    return 0;
}

// Function to initialize file system metadata (Format)
int fs_format() {
    // Initialize Superblock
    superblock.magic = FS_MAGIC;
    superblock.total_blocks = TOTAL_STORAGE_BLOCKS;
    superblock.free_blocks = TOTAL_STORAGE_BLOCKS - FILE_TABLE_SECTORS - 1;
    superblock.file_table_start = 1;  // File Index Table starts at Sector 1
    superblock.data_start = FILE_TABLE_SECTORS + 1;  // Data starts after index table

    // Initialize File Table
    memset(file_table, 0, sizeof(file_table));  // Clear all file entries

    // Write Superblock to storage
    memcpy(storage[0], &superblock, sizeof(Superblock));

    // Write File Table to storage
    memcpy(storage[1], file_table, sizeof(file_table));

    // Write the storage array to the disk image file
    if (write_disk_image() != 0) {
        return -1;
    }

    printf("File system formatted.\n");
    return 0;
}

// Function to mount the file system (Load metadata into RAM)
int fs_mount() {
    file = fopen(DISK_IMAGE, "rb");
    if (!file) {
        printf("Failed to open disk image!\n");
        return -1;
    }

    // Read the entire storage array from the file
    fread(storage, BLOCK_SIZE, TOTAL_STORAGE_BLOCKS, file);
    fclose(file);

    // Read Superblock
    memcpy(&superblock, storage[0], sizeof(Superblock));

    // Validate the file system
    if (superblock.magic != FS_MAGIC) {
        printf("Invalid file system! Please format.\n");
        return -1;
    }

    // Read File Table
    memcpy(file_table, storage[1], sizeof(file_table));
    printf("File system mounted successfully.\n");
    return 0;
}

// Debug function to print file system metadata
void fs_print_info() {
    printf("\n=== File System Info ===\n");
    printf("Magic: 0x%X\n", superblock.magic);
    printf("Total Blocks: %u\n", superblock.total_blocks);
    printf("Free Blocks: %u\n", superblock.free_blocks);
    printf("File Table Start: %u\n", superblock.file_table_start);
    printf("Data Start: %u\n", superblock.data_start);
}