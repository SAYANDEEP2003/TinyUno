#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>
#include "test_os.h"

#define SS_PIN   10
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN  13
#define BLOCK_SIZE 512

uint8_t buffer[BLOCK_SIZE];

// SPI Transfer
uint8_t spi_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

void spi_init() {
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
    DDRB &= ~(1 << MISO_PIN);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

// SD Card Initialization
uint8_t sd_init() {
    PORTB |= (1 << SS_PIN);
    for (uint8_t i = 0; i < 10; i++) spi_transfer(0xFF);
    PORTB &= ~(1 << SS_PIN);
    spi_transfer(0x40); // CMD0
    for (uint8_t i = 0; i < 5; i++) spi_transfer(0x00);
    spi_transfer(0x95); // CRC for CMD0
    uint8_t response;
    for (uint8_t i = 0; i < 10; i++) {
        response = spi_transfer(0xFF);
        if (response == 0x01) break; // Idle state
    }
    PORTB |= (1 << SS_PIN);
    return response;
}

// Read SD block
uint8_t sd_read_block(uint32_t block, uint8_t *buffer) {
    PORTB &= ~(1 << SS_PIN);
    spi_transfer(0x51); // CMD17
    spi_transfer(block >> 24);
    spi_transfer(block >> 16);
    spi_transfer(block >> 8);
    spi_transfer(block);
    spi_transfer(0xFF);
    for (uint16_t i = 0; i < 10000; i++) {
        if (spi_transfer(0xFF) == 0xFE) break; // Data start token
    }
    for (uint16_t i = 0; i < BLOCK_SIZE; i++) buffer[i] = spi_transfer(0xFF);
    spi_transfer(0xFF); // Dummy read for CRC
    spi_transfer(0xFF);
    PORTB |= (1 << SS_PIN);
    return 0;
}

// Find FAT32 partition
uint32_t find_fat32_partition() {
    sd_read_block(0, buffer);
    return *(uint32_t*)&buffer[0x1BE]; // Corrected offset for the first partition
}

// Find root directory
uint32_t find_root_directory(uint32_t partition_start) {
    sd_read_block(partition_start, buffer);
    uint16_t reserved_sectors = *(uint16_t*)&buffer[0x0E];
    uint16_t sectors_per_fat = *(uint16_t*)&buffer[0x16];
    return partition_start + reserved_sectors + (sectors_per_fat * 2);
}

// Find HEX file in FAT32 root directory
uint32_t find_hex_file(uint32_t root_dir_sector) {
    for (uint16_t i = 0; i < 32; i++) {
        sd_read_block(root_dir_sector + i, buffer);
        for (uint16_t j = 0; j < 512; j += 32) {
            if (buffer[j] == 0x00) return 0; // End of directory
            if (memcmp(&buffer[j + 0x00], "OS      HEX", 11) == 0) {
                return *(uint32_t*)&buffer[j + 0x1A]; // Starting cluster
            }
        }
    }
    return 0;
}

// Flash Write
void flash_write_page(uint16_t address, uint8_t *data) {
    boot_page_erase(address);
    boot_spm_busy_wait();
    for (uint8_t i = 0; i < SPM_PAGESIZE; i += 2) {
        uint16_t word = data[i] | (data[i + 1] << 8);
        boot_page_fill(address + i, word);
    }
    boot_page_write(address);
    boot_spm_busy_wait();
    boot_rww_enable(); // Enable RWW section
}

// Read and Flash HEX file
void read_and_flash_hex(uint32_t first_cluster) {
    uint32_t sector = first_cluster;
    uint16_t flash_address = 0x0000;
    while (sector != 0x0FFFFFFF) {
        sd_read_block(sector, buffer);
        for (uint16_t i = 0; i < 512; i += 2) {
            flash_write_page(flash_address, buffer + i);
            flash_address += SPM_PAGESIZE;
        }
        sector++; // Move to the next cluster
    }
}
