#include "mpu6050.h"

static i2c_master_dev_handle_t dev_handle = NULL;

esp_err_t mpu_init(i2c_master_dev_handle_t handle)
{
    dev_handle = handle;

    esp_err_t err;
    uint8_t reg, value;

    uint8_t wake_cmd[] = {MPU_PWR_MGMT_1, 0x00};

    err = i2c_master_transmit(dev_handle, wake_cmd, sizeof(wake_cmd), -1);

    if (err != ESP_OK)
        return err;

    reg = MPU_WHO_AM_I;

    err = i2c_master_transmit_receive(dev_handle, &reg, 1, &value, 1, -1);

    if (err != ESP_OK)
        return err;

    if (value != MPU_ADDR)
        return ESP_FAIL;

    return ESP_OK;
}

esp_err_t mpu_read_gyro(int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t reg = MPU_GYRO_XOUT_H;
    uint8_t data[6];

    esp_err_t err = i2c_master_transmit_receive(dev_handle, &reg, 1, data, 6, -1);

    if (err != ESP_OK)
        return err;

    *gx = (int16_t)((data[0] << 8) | data[1]);
    *gy = (int16_t)((data[2] << 8) | data[3]);
    *gz = (int16_t)((data[4] << 8) | data[5]);

    return ESP_OK;
}
