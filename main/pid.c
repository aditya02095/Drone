#include "pid.h"

void pid_init(pid_t *pid,
              float kp,
              float ki,
              float kd,
              float out_min,
              float out_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->integral = 0.0f;
    pid->previous_error = 0.0f;

    pid->output_min = out_min;
    pid->output_max = out_max;
}

void pid_reset(pid_t *pid)
{
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

float pid_update(pid_t *pid,
                 float setpoint,
                 float measurement,
                 float dt)
{
    float error = setpoint - measurement;

    /* P */
    float P = pid->kp * error;

	/* I */
	pid->integral += error * dt;

	/* Anti-windup */
	if(pid->integral > 100.0f)
	    pid->integral = 100.0f;

	if(pid->integral < -100.0f)
	    pid->integral = -100.0f;

	float I = pid->ki * pid->integral;

    /* D */
    float derivative = (error - pid->previous_error) / dt;
    float D = pid->kd * derivative;

    float output = P + I + D;

    if(output > pid->output_max)
        output = pid->output_max;

    if(output < pid->output_min)
        output = pid->output_min;

    pid->previous_error = error;

    return output;
}