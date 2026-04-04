/**
 * @file hal_gpio.h
 * @brief GPIO Hardware Abstraction Layer for MS4000
 *
 * Provides GPIO control for buttons and LED enable pin.
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "hal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Configure GPIO pin mode
     *
     * @param pin GPIO pin number
     * @param mode Pin mode (input, output, input with pullup)
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_gpio_set_mode(uint8_t pin, hal_gpio_mode_t mode);

    /**
     * @brief Write GPIO pin state
     *
     * @param pin GPIO pin number
     * @param state Pin state (HIGH or LOW)
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_gpio_write(uint8_t pin, hal_gpio_state_t state);

    /**
     * @brief Read GPIO pin state
     *
     * @param pin GPIO pin number
     * @param state Pointer to store pin state
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_gpio_read(uint8_t pin, hal_gpio_state_t *state);

#ifdef __cplusplus
}
#endif

#endif // HAL_GPIO_H
