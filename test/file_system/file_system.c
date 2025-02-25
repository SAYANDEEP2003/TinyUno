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

int fs_create(const char* filename, uint32_t size) {
    // Check if file already exists
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].active && strcmp(file_table[i].filename, filename) == 0) {
            printf("File already exists.\n");
            return -1;
        }
    }

    // Find a free entry in the file table
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].active) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        printf("File table is full.\n");
        return -1;
    }

    // Find free blocks
    uint32_t start_block = superblock.data_start;
    uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (superblock.free_blocks < blocks_needed) {
        printf("Not enough free blocks.\n");
        return -1;
    }

    // Update file table and superblock
    strncpy(file_table[file_index].filename, filename, FILE_NAME_LENGTH);
    file_table[file_index].start_block = start_block;
    file_table[file_index].size = size;
    file_table[file_index].active = 1;

    superblock.free_blocks -= blocks_needed;

    // Write changes to storage
    memcpy(storage[1], file_table, sizeof(file_table));
    memcpy(storage[0], &superblock, sizeof(Superblock));

    if (write_disk_image() != 0) {
        return -1;
    }

    printf("File created: %s\n", filename);
    return 0;
}

int fs_write(const char* filename, const uint8_t* data, uint32_t size) {
    // Find the file in the file table
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].active && strcmp(file_table[i].filename, filename) == 0) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        printf("File not found.\n");
        return -1;
    }

    // Write data to the file's blocks
    uint32_t start_block = file_table[file_index].start_block;
    uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (superblock.free_blocks < blocks_needed) {
        printf("Not enough free blocks.\n");
        return -1;
    }

    for (uint32_t i = 0; i < blocks_needed; i++) {
        uint32_t block_index = start_block + i;
        uint32_t offset = i * BLOCK_SIZE;
        memcpy(storage[block_index], data + offset, (size - offset > BLOCK_SIZE) ? BLOCK_SIZE : size - offset);
    }

    // Update file size
    file_table[file_index].size = size;
    memcpy(storage[1], file_table, sizeof(file_table));

    if (write_disk_image() != 0) {
        return -1;
    }

    printf("Data written to file: %s\n", filename);
    return 0;
}

int fs_read(const char* filename, uint8_t* buffer, uint32_t size) {
    // Find the file in the file table
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].active && strcmp(file_table[i].filename, filename) == 0) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        printf("File not found.\n");
        return -1;
    }

    // Read data from the file's blocks
    uint32_t start_block = file_table[file_index].start_block;
    uint32_t file_size = file_table[file_index].size;
    uint32_t bytes_to_read = (size < file_size) ? size : file_size;

    for (uint32_t i = 0; i < bytes_to_read; i += BLOCK_SIZE) {
        uint32_t block_index = start_block + (i / BLOCK_SIZE);
        uint32_t offset = i % BLOCK_SIZE;
        uint32_t bytes_left = bytes_to_read - i;
        memcpy(buffer + i, storage[block_index] + offset, (bytes_left > BLOCK_SIZE) ? BLOCK_SIZE : bytes_left);
    }

    printf("Data read from file: %s\n", filename);
    return 0;
}

int fs_delete(const char* filename) {
    // Find the file in the file table
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].active && strcmp(file_table[i].filename, filename) == 0) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        printf("File not found.\n");
        return -1;
    }

    // Mark the file as inactive and free its blocks
    uint32_t start_block = file_table[file_index].start_block;
    uint32_t blocks_used = (file_table[file_index].size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    file_table[file_index].active = 0;
    superblock.free_blocks += blocks_used;

    // Write changes to storage
    memcpy(storage[1], file_table, sizeof(file_table));
    memcpy(storage[0], &superblock, sizeof(Superblock));

    if (write_disk_image() != 0) {
        return -1;
    }

    printf("File deleted: %s\n", filename);
    return 0;
}

void fs_list() {
    printf("\n=== File List ===\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].active) {
            printf("File: %s, Size: %u bytes\n", file_table[i].filename, file_table[i].size);
        }
    }
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