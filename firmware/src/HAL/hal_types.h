/**
 * @file hal_types.h
 * @brief Common HAL types and error codes for MS4000 SDK
 *
 * Provides standardized error handling and type definitions
 * for all hardware abstraction layer components.
 */

#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** HAL operation result codes */
    typedef enum
    {
        HAL_OK = 0,              ///< Operation succeeded
        HAL_ERROR = -1,          ///< Generic error
        HAL_ERROR_TIMEOUT = -2,  ///< Operation timed out
        HAL_ERROR_BUSY = -3,     ///< Resource is busy
        HAL_ERROR_INVALID = -4,  ///< Invalid parameter
        HAL_ERROR_NO_DEVICE = -5 ///< Device not found/responding
    } hal_status_t;

    /** GPIO pin configuration */
    typedef enum
    {
        HAL_GPIO_MODE_INPUT = 0,
        HAL_GPIO_MODE_OUTPUT,
        HAL_GPIO_MODE_INPUT_PULLUP
    } hal_gpio_mode_t;

    /** GPIO pin state */
    typedef enum
    {
        HAL_GPIO_LOW = 0,
        HAL_GPIO_HIGH = 1
    } hal_gpio_state_t;

#ifdef __cplusplus
}
#endif

#endif // HAL_TYPES_H
