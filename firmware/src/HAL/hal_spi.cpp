/**
 * @file hal_spi.cpp
 * @brief SPI HAL implementation for MS4000
 *
 * Wraps Arduino SPI library with standardized error handling.
 */

#include "hal_spi.h"
#include <SPI.h>

hal_status_t hal_spi_init(uint32_t frequency)
{
    SPI.begin();
    SPI.setFrequency(frequency);
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    return HAL_OK;
}

hal_status_t hal_spi_write(const uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
    {
        return HAL_ERROR_INVALID;
    }

    SPI.writeBytes(const_cast<uint8_t *>(data), length);
    return HAL_OK;
}

hal_status_t hal_spi_set_frequency(uint32_t frequency)
{
    SPI.setFrequency(frequency);
    return HAL_OK;
}
