#ifndef MAIN_MOTOR_H_
#define MAIN_MOTOR_H_

#include "driver/ledc.h"
#include <stdint.h>

#define MOTOR1      14
#define MOTOR2      27
#define MOTOR3      25
#define MOTOR4      33

#define ESC_FREQ       50
#define ESC_RES        LEDC_TIMER_16_BIT

void motor_init(void);
void motorspeed(int motorpin, uint16_t setspeed);

#endif