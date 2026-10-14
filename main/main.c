#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "mpu6050.h"
#include "motor.h"
#include "pid.h"
#include "receiver.h"
#include "motormixer.h"

#define I2C_SCL_IO      22
#define I2C_SDA_IO      21
#define I2C_PORT        0
#define I2C_FREQ_HZ     100000

#define RECEIVER_CENTER     1500
#define RECEIVER_DEADBAND   20

#define MAX_RATE            200.0f

#define GYRO_SENSITIVITY    131.0f     // ±250 dps

pid_t roll_pid;
pid_t pitch_pid;
pid_t yaw_pid;

i2c_master_bus_handle_t bus_handle = NULL;
i2c_master_dev_handle_t dev_handle = NULL;

static void i2c_master_init(void)
{
    i2c_master_bus_config_t bus_config =
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL_IO,
        .sda_io_num = I2C_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(
        i2c_new_master_bus(
            &bus_config,
            &bus_handle));
}

static void i2c_add_device(void)
{
    i2c_device_config_t dev_cfg =
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MPU_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle,
            &dev_cfg,
            &dev_handle));
}

void app_main(void)
{
    int16_t gx, gy, gz;
    int16_t ax, ay, az;

    uint16_t roll;
    uint16_t pitch;
    uint16_t throttle;
    uint16_t yaw;
    uint16_t aux1;
    uint16_t aux2;

    float desired_roll_rate;
    float desired_pitch_rate;
    float desired_yaw_rate;

    float measured_roll_rate;
    float measured_pitch_rate;
    float measured_yaw_rate;

    float roll_pid_out;
    float pitch_pid_out;
    float yaw_pid_out;

    mixer_output_t motor_output;

    i2c_master_init();
    i2c_add_device();

    ESP_ERROR_CHECK(mpu_init(dev_handle));
    ESP_ERROR_CHECK(receiver_init());

    motor_init();

    pid_init(&roll_pid, 1.0f, 0.0f, 0.1f, -500.0f, 500.0f);
    pid_init(&pitch_pid,1.0f, 0.0f, 0.1f, -500.0f, 500.0f);
    pid_init(&yaw_pid,  1.0f, 0.0f, 0.1f, -500.0f, 500.0f);

    int64_t last_time = esp_timer_get_time();

    TickType_t lastWakeTime = xTaskGetTickCount();

    while (1)
    {
        int64_t current_time = esp_timer_get_time();

        float dt =
            (current_time - last_time) /
            1000000.0f;

        last_time = current_time;

        if (receiver_update() == ESP_OK)
        {
            roll     = receiver_get_roll();
            pitch    = receiver_get_pitch();
            throttle = receiver_get_throttle();
            yaw      = receiver_get_yaw();
            aux1     = receiver_get_aux1();
            aux2     = receiver_get_aux2();
        }

        ESP_ERROR_CHECK(
            mpu_read_gyro(
                &gx,
                &gy,
                &gz));

        ESP_ERROR_CHECK(
            mpu_read_accel(
                &ax,
                &ay,
                &az));

        float roll_input  = (float)roll  - RECEIVER_CENTER;
        float pitch_input = (float)pitch - RECEIVER_CENTER;
        float yaw_input   = (float)yaw   - RECEIVER_CENTER;

        if (fabsf(roll_input) < RECEIVER_DEADBAND)
            roll_input = 0.0f;

        if (fabsf(pitch_input) < RECEIVER_DEADBAND)
            pitch_input = 0.0f;

        if (fabsf(yaw_input) < RECEIVER_DEADBAND)
            yaw_input = 0.0f;

        desired_roll_rate =
            (roll_input / 500.0f) * MAX_RATE;

        desired_pitch_rate =
            (pitch_input / 500.0f) * MAX_RATE;

        desired_yaw_rate =
            (yaw_input / 500.0f) * MAX_RATE;

        measured_roll_rate =
            gx / GYRO_SENSITIVITY;

        measured_pitch_rate =
            gy / GYRO_SENSITIVITY;

        measured_yaw_rate =
            gz / GYRO_SENSITIVITY;

        roll_pid_out =
            pid_update(
                &roll_pid,
                desired_roll_rate,
                measured_roll_rate,
                dt);

        pitch_pid_out =
            pid_update(
                &pitch_pid,
                desired_pitch_rate,
                measured_pitch_rate,
                dt);

        yaw_pid_out =
            pid_update(
                &yaw_pid,
                desired_yaw_rate,
                measured_yaw_rate,
                dt);

        mixer_update(
            throttle,
            roll_pid_out,
            pitch_pid_out,
            yaw_pid_out,
            &motor_output);

        motorspeed(MOTOR1, motor_output.m1);
        motorspeed(MOTOR2, motor_output.m2);
        motorspeed(MOTOR3, motor_output.m3);
        motorspeed(MOTOR4, motor_output.m4);

        vTaskDelayUntil(
            &lastWakeTime,
            pdMS_TO_TICKS(2));
    }
}