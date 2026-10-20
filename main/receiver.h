#ifndef PWM_READER_H
#define PWM_READER_H

#include "driver/gpio.h"
#include "esp_err.h"

esp_err_t pwm_reader_init(gpio_num_t pwm_pin, volatile float *duty_percent);

#endif