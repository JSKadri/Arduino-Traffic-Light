#pragma once
#include <stdint.h>

// Defines Variables
#define RS DDD2 // Register Select is connected to PD4
#define RW DDD4 // Read/Write is connected to PD2
#define EN DDD7 // Enable pin is connected to PD7

#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define PRESCALER1 1024 // 1,8,64,256,1024

// initialize the library with the numbers of the interface pins
void Timer1_init();
void Timer1_Prescaler();
void bigDelay(int occurance);
void delayMs(uint16_t time);

void LCD_command(uint8_t command);
void LCD_data(uint8_t data);
void LCD_init(void);
void LCD_PORTS_init(void);
void LCD_print(const char data[]);