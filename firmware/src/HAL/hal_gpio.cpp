/**
 * @file hal_gpio.cpp
 * @brief GPIO HAL implementation for MS4000
 *
 * Wraps Arduino GPIO functions with standardized error handling.
 */

#include "hal_gpio.h"
#include <Arduino.h>

hal_status_t hal_gpio_set_mode(uint8_t pin, hal_gpio_mode_t mode)
{
    switch (mode)
    {
    case HAL_GPIO_MODE_INPUT:
        pinMode(pin, INPUT);
        break;
    case HAL_GPIO_MODE_OUTPUT:
        pinMode(pin, OUTPUT);
        break;
    case HAL_GPIO_MODE_INPUT_PULLUP:
        pinMode(pin, INPUT_PULLUP);
        break;
    default:
        return HAL_ERROR_INVALID;
    }
    return HAL_OK;
}

hal_status_t hal_gpio_write(uint8_t pin, hal_gpio_state_t state)
{
    digitalWrite(pin, (state == HAL_GPIO_HIGH) ? HIGH : LOW);
    return HAL_OK;
}

hal_status_t hal_gpio_read(uint8_t pin, hal_gpio_state_t *state)
{
    if (state == NULL)
    {
        return HAL_ERROR_INVALID;
    }

    int value = digitalRead(pin);
    *state = (value == HIGH) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    return HAL_OK;
}
