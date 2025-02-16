#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define SS_PIN   10  // Chip Select pin for SD card
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN  13

// SPI Send and Receive
uint8_t spi_transfer(uint8_t data) {
    SPDR = data;                    // Load data into SPI data register
    while (!(SPSR & (1 << SPIF)));   // Wait for transmission to complete
    return SPDR;                     // Return received data
}

// SD Card Initialization
uint8_t sd_init() {
    PORTB |= (1 << SS_PIN);  // Deselect SD card
    for (uint8_t i = 0; i < 10; i++) spi_transfer(0xFF); // Send 80 clock pulses

    PORTB &= ~(1 << SS_PIN); // Select SD card
    spi_transfer(0x40);      // CMD0 (Go to idle state)
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x95); // Correct CRC for CMD0

    uint8_t response;
    for (uint8_t i = 0; i < 10; i++) {
        response = spi_transfer(0xFF);
        if (response == 0x01) break; // SD card is in idle state
    }

    PORTB |= (1 << SS_PIN); // Deselect SD card
    return response;
}

// Read a single block from SD card (512 bytes)
uint8_t sd_read_block(uint32_t block, uint8_t *buffer) {
    PORTB &= ~(1 << SS_PIN); // Select SD card
    spi_transfer(0x51);      // CMD17 (Read single block)
    
    // Send block address
    spi_transfer(block >> 24);
    spi_transfer(block >> 16);
    spi_transfer(block >> 8);
    spi_transfer(block);
    
    spi_transfer(0xFF);  // Dummy CRC

    uint8_t response;
    for (uint16_t i = 0; i < 10000; i++) { // Wait for start token (0xFE)
        response = spi_transfer(0xFF);
        if (response == 0xFE) break;
    }

    // Read 512 bytes of data
    for (uint16_t i = 0; i < 512; i++) {
        buffer[i] = spi_transfer(0xFF);
    }

    spi_transfer(0xFF); // Ignore CRC
    spi_transfer(0xFF);

    PORTB |= (1 << SS_PIN); // Deselect SD card
    return 0;
}

int main() {
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN); // Set SPI pins as output
    DDRB &= ~(1 << MISO_PIN); // Set MISO as input
    
    // Initialize SPI
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // Enable SPI, Master mode, Fclk/16
    
    // Initialize SD card
    if (sd_init() != 0x01) {
        // SD card initialization failed
        while (1);
    }

    uint8_t buffer[512];
    if (sd_read_block(0, buffer) == 0) {
        // Successfully read block
    }

    while (1);
}