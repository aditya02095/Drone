#include "motor.h"
#include "esp_err.h"

static ledc_channel_t get_channel(int pin)
{
    switch(pin)
    {
        case MOTOR1:
            return LEDC_CHANNEL_0;

        case MOTOR2:
            return LEDC_CHANNEL_1;

        case MOTOR3:
            return LEDC_CHANNEL_2;

        case MOTOR4:
            return LEDC_CHANNEL_3;

        default:
            return LEDC_CHANNEL_0;
    }
}

void motor_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = ESC_RES,
        .freq_hz = ESC_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    int pins[4] = {MOTOR1, MOTOR2, MOTOR3, MOTOR4};

    for(int i = 0; i < 4; i++)
    {
        ledc_channel_config_t channel = {
            .gpio_num = pins[i],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = get_channel(pins[i]),
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        };

        ESP_ERROR_CHECK(ledc_channel_config(&channel));
    }
}

void motorspeed(int motorpin, uint16_t setspeed)
{
    if(setspeed < 1000)
        setspeed = 1000;

    if(setspeed > 2000)
        setspeed = 2000;

    uint32_t duty = ((uint32_t)setspeed * 65535) / 20000;

    ledc_channel_t channel = get_channel(motorpin);

    ESP_ERROR_CHECK(
        ledc_set_duty(
            LEDC_LOW_SPEED_MODE,
            channel,
            duty));

    ESP_ERROR_CHECK(
        ledc_update_duty(
            LEDC_LOW_SPEED_MODE,
            channel));
}