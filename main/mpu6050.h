#ifndef MPU6050_H
#define MPU6050_H

#include "esp_err.h"
#include "driver/i2c_master.h"
#include <stdint.h>

#define MPU_ADDR           0x68
#define MPU_PWR_MGMT_1     0x6B
#define MPU_WHO_AM_I       0x75
#define MPU_GYRO_XOUT_H    0x43
#define MPU_ACCEL_XOUT_H   0x3B

esp_err_t mpu_init(i2c_master_dev_handle_t dev_handle);

esp_err_t mpu_read_gyro(
    int16_t *gx,
    int16_t *gy,
    int16_t *gz);

esp_err_t mpu_read_accel(
    int16_t *ax,
    int16_t *ay,
    int16_t *az);

#endif