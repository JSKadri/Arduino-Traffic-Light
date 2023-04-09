/*
* Jawad Kadri 101147056
* Noah Arcand Da Silva 101147756
* Justin Franko 101156681
* April 11, 2022
*/

#include <Arduino.h>
#include <avr_debugger.h>
#include <stdio.h>
#include "Servo.h"
#include "LCD.h"
#include "SPI.h"


// Defines Variables
#define RED_LED DDC2
#define YLW_LED DDC3
#define GRN_LED DDC4
#define BTN DDC1

#define BUZZ DDC0
#define ServoX DDD5 // Pin 9 is connected to OC1A pin
#define ServoY DDD6 // Pin 10 is connected to OC1B pin

const uint8_t numArray[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D,
					0x7D, 0x07, 0x7F, 0x6F, 0x77, 0xFF,
					0x39, 0xBF, 0x79, 0x71};

uint8_t delayTime;
boolean buttonPressed;

// Function prototypes
boolean LCD_blink(uint8_t direction, boolean blinkOccured);
uint8_t LCD_defaultTraffic(uint8_t r);
void blinkYellow(uint8_t delayTime, uint8_t waitTimer, uint8_t direction, boolean blinkOccured);
void redOn();
void digitSend(uint8_t num);
void LCS(boolean upDown);

int main() {	
	// Add at the beginning of each project to enable debugging
	#ifdef __DEBUG__
		dbg_start();
	#endif

	// Setting LED Pins as Output
	DDRC |= (1 << RED_LED) | (1 << YLW_LED) | (1 << GRN_LED) | (1 << BTN) | (1 << BUZZ);
	DDRD |= (1 << ServoX) | (1 << ServoY);
	DDRB |= (1 << dataPin) | (1 << latchPin) | (1 << clockPin);
 	PORTB &= ~((1 << dataPin)|(1 << latchPin)|(1 << clockPin));
	
	
	Timer0_PWM_Init();	// initialize timer 0 for servo
	Timer1_init();		// initialize timer 1 for LCD (delayMs)

	// initilaize SPI for Switch Registers
	SPI_init();

	// initialize LCD controller
	LCD_init();

	PCICR |= (1 << PCIE1);    // set PCIE1 to enable PCMSK1 scan
	PCMSK1 |= (1 << PCINT9);  // set PCINT9 to trigger an interrupt on state change 
	sei();                    // turn on interrupts

	
	// This is an infinite loop that runs forever  
	while(1) 
	{
		delayTime = 30;			// Default delay time
		buttonPressed = false;  // Reset buttonPressed
		uint8_t direction = LCD_defaultTraffic(0);	// Display default car traffic
		boolean blinkOccured = false;				// Reset blinkOccured
		
		PORTC ^= (1 << GRN_LED);  // Turn on Green Light
		for(int i = 1; i <= delayTime; i++) {
			blinkOccured = LCD_blink(direction, blinkOccured); // Begin traffic flow
			delayMs(1000);	// Delay for 1 second
		}
     	blinkYellow(10,9, direction, blinkOccured);	// Warn cars red light in 10 seconds
      	redOn();                 					// Set red light for 15 seconds
	}
}

/* Interupt Button Was Pressed */
ISR (PCINT1_vect) {
	if(!buttonPressed) {
		delayTime = round(delayTime/2);	// Decrease delay time by half
		buttonPressed = true; 			// Set buttonPressed to true
		
	}
}

/* All the data for our LCD animation */
boolean LCD_blink(uint8_t direction, boolean blinkOccured) {
	LCD_command(0x01);	// Clear LCD
	LCD_command(0x02);	// Return Home
	if(direction == 1) {
		if(!blinkOccured) {
			LCD_print("  | |     |   "); // Display cars
			LCD_data(0xFC);				// Display person
			LCD_print("^"); 			// Display straight arrow
			LCD_command(0xC0);			// Move to second line
			LCD_print("|     | |   |"); // // Display cars
			blinkOccured = true;
		} else {
			LCD_print("|     | |   | "); // Display cars
			LCD_data(0xFC);				// Display person
			LCD_print("^"); 			// Display straight arrow
			LCD_command(0xC0);			// Move to second line
			LCD_print("  | |     |  "); // Display cars
			blinkOccured = false;
		}
	} else if (direction == 2) {
		if(!blinkOccured) {
			LCD_print("` `     |   | ");// Display cars
			LCD_data(0xFC);				// Display person
			LCD_data(0x7F); 			// Display left arrow
			LCD_command(0xC0);			// Move to second line
			LCD_print("    | |   |  "); // Display cars
			blinkOccured = true;
		} else {
			LCD_print("    | |   |   ");// Display car traffic
			LCD_data(0xFC);				// Display person
			LCD_data(0x7F);				// Display left arrow
			LCD_command(0xC0);			// Move to second line
			LCD_print("| |     |   |"); // Display cars
			blinkOccured = false;
		}
	} else if (direction == 3) {
		if(!blinkOccured) {
			LCD_print("  |   |   / / ");// Display cars
			LCD_data(0xFC);				// Display person
			LCD_data(0x7E); 			// Display right arrow
			LCD_command(0xC0);			// Move to second line
			LCD_print("|   |   |    "); // Display cars
			blinkOccured = true;
		} else {
			LCD_print("|   |   |     ");// Display cars
			LCD_data(0xFC);				// Display person
			LCD_data(0x7E); 			// Display right arrow
			LCD_command(0xC0);			// Move to second line
			LCD_print("  |   |   | |"); // Display cars
			blinkOccured = false;
		}
	} else if (direction == 4) {	// Train
		if(!blinkOccured) {
			LCD_print("        ");
			LCD_data(0x7F); 			// Display Train Head
			LCD_print("------");			// Display train cargo
			LCD_data(0xA5); 			// Display stop
			LCD_command(0xC0);			// Move to second line
			LCD_print("| | | | | | |"); // Display cars
			blinkOccured = true;
		} else {
			LCD_data(0x7F); 			// Display Train Head
			LCD_print("--------------");// Display Crosswalk
			LCD_data(0xA5); 			// Display stop
			LCD_command(0xC0);			// Move to second line
			LCD_print("| | | | | | |"); // Display cars
			blinkOccured = false;
		}
		
	} else if (direction == 5) {	// Stop
		if(!blinkOccured) {
			LCD_print("   "); 			// Display Spacing
			for(int j = 0; j < 6; j++) {
				LCD_data(0xFC);			// Display person
				LCD_print(" "); 		// Display Spacing
			}
			
			LCD_data(0xA5); 			// Display stop sign
			LCD_command(0xC0);			// Move to second line
			LCD_print("| | | | | | |"); // Display cars
			blinkOccured = true;
		} else {
			for(int j = 0; j < 6; j++) {
				LCD_print(" "); 		// Display Spacing
				LCD_data(0xFC);			// Display person
			}
			LCD_print("   "); 			// Display Spacing
			LCD_data(0xA5); 			// Display stop
			LCD_command(0xC0);			// Move to second line
			LCD_print("| | | | | | |"); // Display cars
			blinkOccured = false;
		}	
	}
	return blinkOccured;	// Return blinkOccured
}

/* Displays the static view of the traffic */
uint8_t LCD_defaultTraffic(uint8_t r) {
	LCD_command(0x01);	// Clear LCD
	LCD_command(0x02);	// Return Home
	if(r == 0) {
		r = random(1, 4); // Random number between 1 and 3
	}
	
	if(r == 1) {
		LCD_print("| | | | | | |"); // Display cars
		LCD_data(0xFC);				// Display person
		LCD_print("^"); 			// Display straight arrow
		LCD_command(0xC0);			// Move to second line
		LCD_print("| | | | | | |"); // Display cars

	} else if (r == 2) {
		LCD_print("` ` | | | | | ");// Display cars
		LCD_data(0xFC);				// Display person
		LCD_data(0x7F); 			// Display left arrow
		LCD_command(0xC0);			// Move to second line
		LCD_print("| | | | | | |"); // Display cars

	} else if (r == 3) {
		LCD_print("| | | | | / / ");// Display cars
		LCD_data(0xFC);				// Display person
		LCD_data(0x7E); 			// Display right arrow
		LCD_command(0xC0);			// Move to second line
		LCD_print("| | | | | | |"); // Display cars
	} else if (r == 4) {
		LCD_data(0x7F); 			// Display Train Head
		LCD_print("--------------");// Display Crosswalk
		LCD_data(0xA5); 			// Display stop
		LCD_command(0xC0);			// Move to second line
		LCD_print("| | | | | | |"); // Display cars
	} else if (r == 5) {
		LCD_print("              ");// Display Crosswalk
		LCD_data(0xFC);				// Display person
		LCD_data(0xA5); 			// Display stop
		LCD_command(0xC0);			// Move to second line
		LCD_print("| | | | | | |"); // Display cars
	} else if (r == 6) {
		LCD_print("Please Wait   ");// Display Crosswalk
		LCD_data(0xFC);				// Display person
		LCD_data(0xA5); 			// Display stop
		LCD_command(0xC0);			// Move to second line
		LCD_print("| | | | | | |"); // Display cars
	}
	return r;
}

// Yellow light
void blinkYellow(uint8_t occurance, uint8_t waitDigit, uint8_t direction, boolean blinkOccured) {
  for(int i = 0; i < occurance; i++) {   // Loop for 5 seconds
  	blinkOccured = LCD_blink(direction, blinkOccured); // Continue traffic flow
    digitSend(waitDigit);             	 // Output the current number (to the screen)
    PORTC ^= (1 << YLW_LED);             // Turn on Yellow Light
    delayMs(1000);                       // Delay for 1 second 
    waitDigit--;                         // Reduce the wait timer
  }
  PORTC ^= (1 << GRN_LED);  // Turn off Green Light
}

// Turn on Red light for 15 seconds
void redOn() {
	boolean blinkOccured = false;	// Blink not occured

	PORTC ^= (1 << RED_LED);  // Turn on Red Light

	LCD_defaultTraffic(6); 	// Display Waiting
	LCS(false);			 	// Servo Down
	LCD_defaultTraffic(4);	// Display Train Section
	for(int i = 1; i <= 15; i++) {	// Loop for 10 seconds
		blinkOccured = LCD_blink(4, blinkOccured); // Begin traffic flow
		delayMs(1000);							  // Delay for 1 second 	
	}

	LCD_defaultTraffic(6); // Display Waiting
	LCS(true);	// Servo Up

	blinkOccured = false;
	for(int i = 1; i <= 15; i++) {	// Loop for 15 seconds
	  	PORTC ^= (1 << BUZZ);	 	// Turn on Buzzer (Safe to cross)
		blinkOccured = LCD_blink(5, blinkOccured); // Begin traffic flow
		delayMs(1000);					// Delay for 1 second 	
	}
	PORTC ^= (1 << BUZZ);	 	// Turn on Buzzer (Safe to cross)
	PORTC ^= (1 << RED_LED);  // Turn off Red Light
}

/* Send the digit to the 7-segment LCD */
void digitSend(uint8_t num) {
	SPI_begin();
	SPI_send(0x00, numArray[num]); // Note first argument is for LCD data ONLY
	SPI_end();
}

/*  Level Crossing Sign - A servo motor that 
	acts as the line stopper so cars don't 
	cross during an incoming train 		  */
void LCS(boolean upDown) {
	if(upDown)  // true (Up)
	{	
		for(uint8_t i = 90; i > 0; i--) {
			OCR0B = (uint8_t) ((i * 17.0) / 180.0) + 3; // (0<->180 is 3 <-> 20)
			delayMs(80);								// Delay for 25 milliseconds
		}		
	}
	else {	// false (Down)
		for(uint8_t i = 1; i < 90; i++) {
			OCR0B = (uint8_t) ((i * 17.0) / 180.0) + 3; // (0<->180 is 3 <-> 20)
			delayMs(80);								// Delay for 25 milliseconds
		}
	}
}