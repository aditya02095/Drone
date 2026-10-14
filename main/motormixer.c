#include "motormixer.h"

static uint16_t constrain_motor(float value)
{
    if(value < MOTOR_MIN_PWM)
        return MOTOR_MIN_PWM;

    if(value > MOTOR_MAX_PWM)
        return MOTOR_MAX_PWM;

    return (uint16_t)value;
}

void mixer_update(
    uint16_t throttle,
    float roll,
    float pitch,
    float yaw,
    mixer_output_t *motor)
{
    float m1;
    float m2;
    float m3;
    float m4;

    /*
     * X Configuration
     *
     *         Front
     *
     *      M1(CCW)    M2(CW)
     *
     *      M4(CW)     M3(CCW)
     *
     *         Rear
     */

    m1 = throttle + pitch + roll - yaw;
    m2 = throttle + pitch - roll + yaw;
    m3 = throttle - pitch - roll - yaw;
    m4 = throttle - pitch + roll + yaw;

    motor->m1 = constrain_motor(m1);
    motor->m2 = constrain_motor(m2);
    motor->m3 = constrain_motor(m3);
    motor->m4 = constrain_motor(m4);
}