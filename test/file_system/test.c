#include <stdio.h>
#include "file_system.h"

int main() {
    printf("Starting File System Test...\n");

    // Step 1: Format the file system
    printf("\n[TEST] Formatting File System...\n");
    fs_format();

    // Step 2: Mount the file system
    printf("\n[TEST] Mounting File System...\n");
    if (fs_mount() == 0) {
        printf("Mount successful.\n");
    } else {
        printf("Mount failed!\n");
        return -1;
    }

    // Step 3: Print File System Info
    printf("\n[TEST] Printing File System Info...\n");
    fs_print_info();

    // Step 4: Create a file
    printf("\n[TEST] Creating a file...\n");
    fs_create("test1.txt", 256);

    // Step 5: Write to the file
    uint8_t data[] = "Hello, RTOS!";
    printf("\n[TEST] Writing to the file...\n");
    fs_write("test1.txt", data, sizeof(data));

    // Step 6: Read from the file
    uint8_t buffer[256];
    printf("\n[TEST] Reading from the file...\n");
    fs_read("test1.txt", buffer, sizeof(data));
    printf("Read Data: %s\n", buffer);

    // Step 7: List all files
    printf("\n[TEST] Listing all files...\n");
    fs_list();

    // Step 8: Delete the file
    printf("\n[TEST] Deleting the file...\n");
    fs_delete("test1.txt");

    // Step 9: List all files again
    printf("\n[TEST] Listing all files after deletion...\n");
    fs_list();

    printf("\nTest Completed.\n");
    return 0;
}
