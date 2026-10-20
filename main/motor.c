#include "motor.h"

#include <stdbool.h>

#include "driver/gpio.h"
#include "driver/ledc.h"

#define PWM_FREQUENCY_HZ  100
#define PWM_RESOLUTION    LEDC_TIMER_14_BIT
#define PWM_MAX_DUTY      ((1U << 14) - 1U)

#define PWM_TIMER         LEDC_TIMER_0
#define PWM_MODE          LEDC_LOW_SPEED_MODE

static const int pwm_pins[] = {25, 26, 27, 33};

static const ledc_channel_t pwm_channels[] = {
    LEDC_CHANNEL_0,
    LEDC_CHANNEL_1,
    LEDC_CHANNEL_2,
    LEDC_CHANNEL_3
};

#define PWM_CHANNEL_COUNT 4

static bool timer_initialized = false;
static bool channel_initialized[PWM_CHANNEL_COUNT] = {false};

static int find_pwm_channel(int pwm_pin)
{
    for (int i = 0; i < PWM_CHANNEL_COUNT; i++) {
        if (pwm_pins[i] == pwm_pin) {
            return i;
        }
    }

    return -1;
}

static esp_err_t initialize_pwm_timer(void)
{
    if (timer_initialized) {
        return ESP_OK;
    }

    ledc_timer_config_t timer_config = {
        .speed_mode       = PWM_MODE,
        .duty_resolution  = PWM_RESOLUTION,
        .timer_num        = PWM_TIMER,
        .freq_hz          = PWM_FREQUENCY_HZ,
        .clk_cfg          = LEDC_AUTO_CLK
    };

    esp_err_t err = ledc_timer_config(&timer_config);

    if (err == ESP_OK) {
        timer_initialized = true;
    }

    return err;
}

esp_err_t motor_set(int pwm_pin, float duty_percent)
{
    if (duty_percent < 0.0f || duty_percent > 100.0f) {
        return ESP_ERR_INVALID_ARG;
    }

    int index = find_pwm_channel(pwm_pin);

    if (index < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = initialize_pwm_timer();

    if (err != ESP_OK) {
        return err;
    }

    if (!channel_initialized[index]) {
        ledc_channel_config_t channel_config = {
            .gpio_num   = pwm_pin,
            .speed_mode = PWM_MODE,
            .channel    = pwm_channels[index],
            .intr_type  = LEDC_INTR_DISABLE,
            .timer_sel  = PWM_TIMER,
            .duty       = 0,
            .hpoint     = 0
        };

        err = ledc_channel_config(&channel_config);

        if (err != ESP_OK) {
            return err;
        }
        channel_initialized[index] = true;
	}

    uint32_t duty = (uint32_t)((duty_percent / 100.0f) * PWM_MAX_DUTY);

    err = ledc_set_duty(
        PWM_MODE,
        pwm_channels[index],
        duty
    );
    if (err != ESP_OK) {
        return err;
    }
    return ledc_update_duty(
        PWM_MODE,
        pwm_channels[index]
    );
}