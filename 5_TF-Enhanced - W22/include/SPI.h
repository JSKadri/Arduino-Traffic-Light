#pragma once
#include <Arduino.h>

#define clockPin DDB5 // SRCLK of 74HC595 is connected to PB5 (SCK pin)
#define dataPin DDB3  // SER of 74HC595 is connected to PB3 (MOSI pin)
#define latchPin DDB2 // RCLK of 74HC595 is connected to PB2 (SS pin)

void SPI_init(void);
void SPI_send(uint8_t cData1, uint8_t cData2);
void SPI_begin();
void SPI_end();

