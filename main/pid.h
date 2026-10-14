#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct
{
    float kp;
    float ki;
    float kd;

    float integral;
    float previous_error;

    float output_min;
    float output_max;

} pid_t;

/* Initialize a PID controller */
void pid_init(pid_t *pid,
              float kp,
              float ki,
              float kd,
              float out_min,
              float out_max);

/* Reset integral and derivative history */
void pid_reset(pid_t *pid);

/* Compute one PID update */
float pid_update(pid_t *pid,
                 float setpoint,
                 float measurement,
                 float dt);

#endif