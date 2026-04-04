/**
 * @file hal_i2c.cpp
 * @brief I2C HAL implementation for MS4000
 *
 * Wraps Arduino Wire library with standardized error handling.
 */

#include "hal_i2c.h"
#include <Wire.h>

hal_status_t hal_i2c_init(uint8_t sda_pin, uint8_t scl_pin, uint32_t frequency)
{
    Wire.begin(sda_pin, scl_pin);
    Wire.setClock(frequency);
    return HAL_OK;
}

hal_status_t hal_i2c_write_register(uint8_t device_addr, uint8_t reg_addr, uint8_t data)
{
    Wire.beginTransmission(device_addr);
    Wire.write(reg_addr);
    Wire.write(data);

    uint8_t error = Wire.endTransmission();
    return (error == 0) ? HAL_OK : HAL_ERROR;
}

hal_status_t hal_i2c_read_register(uint8_t device_addr, uint8_t reg_addr, uint8_t *data)
{
    if (data == NULL)
    {
        return HAL_ERROR_INVALID;
    }

    Wire.beginTransmission(device_addr);
    Wire.write(reg_addr);
    uint8_t error = Wire.endTransmission();

    if (error != 0)
    {
        return HAL_ERROR;
    }

    uint8_t bytes_read = Wire.requestFrom(device_addr, (uint8_t)1);
    if (bytes_read != 1)
    {
        return HAL_ERROR_NO_DEVICE;
    }

    *data = Wire.read();
    return HAL_OK;
}

hal_status_t hal_i2c_read_registers(uint8_t device_addr,
                                    uint8_t reg_addr,
                                    uint8_t *data,
                                    uint8_t length)
{
    if (data == NULL || length == 0)
    {
        return HAL_ERROR_INVALID;
    }

    Wire.beginTransmission(device_addr);
    Wire.write(reg_addr);
    uint8_t error = Wire.endTransmission();

    if (error != 0)
    {
        return HAL_ERROR;
    }

    uint8_t bytes_read = Wire.requestFrom(device_addr, length);
    if (bytes_read != length)
    {
        return HAL_ERROR_NO_DEVICE;
    }

    for (uint8_t i = 0; i < length; i++)
    {
        data[i] = Wire.read();
    }

    return HAL_OK;
}
