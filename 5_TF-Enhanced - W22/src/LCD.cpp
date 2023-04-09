#include "SPI.h"
#include "LCD.h"
#include <Arduino.h>


void Timer1_init()
{
	// initialize timer1
	TCCR1A &= ~((1 << WGM10) | (1 << WGM11)); // set Timer 1 as normal mode
	TCCR1B &= ~((1 << WGM12) | (1 << WGM13));
	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); // Disable Timer 1 until needed
}

/* Function to set CS12:CS10 bits from TCCR1B based on which Prescaler is needed */
void Timer1_Prescaler()
{
    TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); // Disable Timer 0 until needed

    switch (PRESCALER1)
    {
    case 1:
        // N = 1      F =  121.951Hz     T = 8.199msec    // 5msec Oscilliscope scale
        TCCR1B |= (1 << CS10); // setting prescaler to 1
        break;
    case 8:
        // N = 8      F =  15.25Hz     T =  0.065sec    // 20msec Oscilliscope scale
        TCCR1B |= (1 << CS11); // setting prescaler to 8
        break;
    case 64:
        // N = 64      F =  1.959Hz     T = 0.510sec    // 0.1sec Oscilliscope scale
        TCCR1B |= (1 << CS10) | (1 << CS11); // setting prescaler to 64
        break;
    case 256:
        // N = 256      F =  0.5Hz     T = 2sec    // 0.5sec Oscilliscope scale
        TCCR1B |= (1 << CS12); // setting prescaler to 256
        break;
    case 1024:
        // N = 1024      F =  0.116Hz     T = 8.6sec    // 1sec Oscilliscope scale
        TCCR1B |= (1 << CS10) | (1 << CS12); // setting prescaler to 1024
        break;
    default:
        TCCR1B |= (1 << CS10); // setting prescaler to 1
    }
}

void delayMs(uint16_t time)
{
    if(time > 2000) {
        time = 2000;
    }
	// Note you need to cast time as a float because you cannot divide an int. 
	// Also use float not doubles because doubles takes up more space. float (32 bits) -> double (64 bits)
	float delay = ((float)time/1000);	// Convert time from miliseconds to seconds
	// 2^16 - ( (16000000 * time) / PRESCALER )
	uint16_t preloadTimer = 65536.0 - ( (16000000.0 * delay) / PRESCALER1 );

	TCNT1H = (preloadTimer & 0xFF00) >> 8;    // clear lower 8 byte then shift 8 bits to the right
	TCNT1L = preloadTimer & 0x00FF;			// clear higher 8 byte of timer
	
	Timer1_Prescaler(); // set the prescaler bits

	// wait for the overflow event
	while ((TIFR1 & (1 << TOV1)) == 0)
		;

	// reset the overflow flag
	TIFR1 |= (1 << TOV1);

	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); // Disable Timer 1 until needed
}

void bigDelay(int occurance) {
    for (int i = 0; i < occurance; i++) {
        delayMs(1000);
    }
}

// initialize port pins then initialize LCD controller
void LCD_init(void)
{
    LCD_PORTS_init();

    delayMs(30);     /* initialization sequence */
    LCD_command(0x30); /* 1 line and 5x7 character */
    delayMs(10);
    LCD_command(0x30);
    delayMs(1);
    LCD_command(0x30);

    LCD_command(0x38); /* set 8-bit data, 2-line, 5x7 font */
    LCD_command(0x06); /* move cursor right after each char */
    LCD_command(0x01); /* clear screen, move cursor to home */
    LCD_command(0x0F); /* turn on display, cursor blinking */
}

// Initialize ports to output mode
void LCD_PORTS_init(void)
{
    /* PB4 for LCD R/S */
    /* PB3 for LCD R/W */
    /* PB2 for LCD EN */
    DDRD |= (1 << RS) | (1 << RW) | (1 << EN); /* set pin output mode */
    PORTD &= ~((1 << RW) | (1 << EN));         /* turn off EN and R/W */
}

// Send Commands to LCD
void LCD_command(uint8_t command)
{
    PORTD &= ~((1 << RS) | (1 << RW)); /* RS = 0, R/W = 0 */
    
    SPI_begin();
    SPI_send(command, 0x00); /* put command on data bus */
    SPI_end();

    PORTD |= (1 << EN);  /* pulse E high */
    PORTD &= ~(1 << EN); /* clear E */

    if (command < 4)
    {
        delayMs(2); /* command 1 and 2 needs up to 1.64ms */
    }
    else
    {
        delayMs(1); /* all others 40 us */
    }
}

// Send Data to LCD
void LCD_data(uint8_t data)
{
    PORTD |= (1 << RS);  /* RS = 1 */
    PORTD &= ~(1 << RW); /* R/W = 0 */
    
    SPI_begin();
    SPI_send(data, 0x00);      /* put data on data bus */
    SPI_end();
    
    PORTD |= (1 << EN);  /* pulse E high */
    PORTD &= ~(1 << EN); /* clear E */

    delayMs(1);
}

// Send a string to LCD
void LCD_print(const char data[])
{
    for (int i = 0; data[i] != '\0'; i++)
        LCD_data(data[i]);
}