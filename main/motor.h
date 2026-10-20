#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H

#include "esp_err.h"

esp_err_t motor_set(int pwm_pin, float duty_percent);

#endif
