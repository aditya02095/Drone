#include "receiver.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_attr.h"

static volatile float *s_duty_percent;
static gpio_num_t s_pwm_pin;

static volatile int64_t s_rising_time = 0;
static volatile int64_t s_falling_time = 0;

static void IRAM_ATTR pwm_isr_handler(void *arg)
{
    int64_t now = esp_timer_get_time();
    int level = gpio_get_level(s_pwm_pin);

    if (level == 1) {
        int64_t previous_rising = s_rising_time;
        int64_t previous_falling = s_falling_time;

        if (previous_rising > 0 &&
            previous_falling >= previous_rising &&
            now > previous_rising) {

            int64_t period = now - previous_rising;
            int64_t high_time =
                previous_falling - previous_rising;

            if (period > 0 && high_time <= period) {
                *s_duty_percent =
                    (100.0f * (float)high_time) /
                    (float)period;
            }
        }

        s_rising_time = now;
    } else {
        s_falling_time = now;
    }
}

esp_err_t pwm_reader_init(gpio_num_t pwm_pin, volatile float *duty_percent)
{
    if (duty_percent == NULL ||
        pwm_pin < 0 ||
        pwm_pin >= GPIO_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    s_pwm_pin = pwm_pin;
    s_duty_percent = duty_percent;

    s_rising_time = 0;
    s_falling_time = 0;
    *s_duty_percent = 0.0f;

    gpio_config_t config = {
        .pin_bit_mask = 1ULL << pwm_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) {
        return err;
    }

    err = gpio_install_isr_service(0);

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
	{
        return err;
    }

    err = gpio_isr_handler_add(pwm_pin, pwm_isr_handler, NULL);

    return err;
}