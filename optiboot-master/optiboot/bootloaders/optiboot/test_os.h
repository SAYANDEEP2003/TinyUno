#ifndef TEST_OS_H
#define TEST_OS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void spi_init(void);
uint8_t sd_init(void);
uint8_t sd_read_block(uint32_t block, uint8_t *buffer);
uint32_t find_fat32_partition(void);
uint32_t find_root_directory(uint32_t partition_start);
uint32_t find_hex_file(uint32_t root_dir_sector);
void read_and_flash_hex(uint32_t first_cluster);

#ifdef __cplusplus
}
#endif

#endif // TEST_OS_H