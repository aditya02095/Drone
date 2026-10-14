#ifndef MOTORMIXER_H
#define MOTORMIXER_H

#include <stdint.h>

#define MOTOR_MIN_PWM    1000
#define MOTOR_MAX_PWM    2000

typedef struct
{
    uint16_t m1;
    uint16_t m2;
    uint16_t m3;
    uint16_t m4;
} mixer_output_t;

void mixer_update(
    uint16_t throttle,
    float roll,
    float pitch,
    float yaw,
    mixer_output_t *motor);

#endif