#pragma once
#include <Arduino.h>

#define PRESCALER 1024 // 1,8,64,256,1024
// Function prototypes
void Timer0_PWM_Init();
void Timer0_Prescaler();
void Servo_Move(uint16_t X_angle, uint16_t Y_angle);
void Servo_Move_X(uint16_t X_angle);
void Servo_Move_Y(uint16_t Y_angle);