#include "Servo.h"
#include <Arduino.h>

void Servo_Move(uint16_t X_angle, uint16_t Y_angle)
{
    OCR1A = ((X_angle * 128.0) / 180.0)+22; // 0 degx = (X-A)/(B-A) * (D-C) + C where
    OCR1B = ((Y_angle * 128.0) / 180.0)+22; // 0 deg
}

void Servo_Move_X(uint16_t X_angle)
{
    OCR1A = ((X_angle * 128.0) / 180.0)+22; // 0 degx = (X-A)/(B-A) * (D-C) + C where
}
void Servo_Move_Y(uint16_t Y_angle)
{
    OCR1B = ((Y_angle * 128.0) / 180.0)+22; // 0 deg
}


void Timer0_PWM_Init()
{
	TCCR0B = 0x00; // Disable Timer 0 until needed

	TCCR0A = 0x00; // initialize TCCR0A to zeros

	OCR0A = 156; // set PWM for 50% duty cycle
	OCR0B = 3;	 // set PWM for 50% duty cycle (between 3 and 20)
	// Set as PWM Phase correct (Mode 5), non-inverting for OC0A and OC0B
	// TCCR0A |=  (1 << WGM00) | (1 << COM0A1) | (1 << COM0A0)|(1 << COM0B1) | (1 << COM0B0);
	TCCR0A |= (1 << WGM00) | (1 << COM0B1);
	TCCR0B |= (1 << WGM02);
	Timer0_Prescaler(); // set prescaler and start timer
}

/* Function to set CS02:CS00 bits from TCCR0B based on which Prescaler is needed */
void Timer0_Prescaler()
{
	TCCR0B &= ~((1 << CS00) | (1 << CS01) | (1 << CS02)); // disable timer 0
	switch (PRESCALER)
	{
	case 1:
		// N = 1      F =  373Hz     T = 2.68msec    // 0.5msec Oscilliscope scale
		TCCR0B |= (1 << CS00); // setting prescaler to 1
		break;
	case 8:
		// N = 8      F =  1.234kHz     T = 0.809msec    // 0.2msec Oscilliscope scale
		TCCR0B |= (1 << CS01); // setting prescaler to 8
		break;
	case 64:
		// N = 64      F =  11.718Hz     T = 0.085sec    // 20msec Oscilliscope scale
		TCCR0B |= (1 << CS00) | (1 << CS01); // setting prescaler to 64
		break;
	case 256:
		// N = 256      F =  1.5Hz     T = 0.682sec    // 0.2sec Oscilliscope scale
		TCCR0B |= (1 << CS02); // setting prescaler to 256
		break;
	case 1024:
		// N = 1024      F =  0.366 Hz     T = 2.728sec    // 1sec Oscilliscope scale
		TCCR0B |= (1 << CS00) | (1 << CS02); // setting prescaler to 1024
		break;
	default:
		TCCR0B |= (1 << CS00); // setting prescaler to 1
	}
}

