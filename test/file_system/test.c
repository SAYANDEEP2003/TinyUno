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

    printf("\nTest Completed.\n");
    return 0;
}