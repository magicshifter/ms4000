/**
 * @file hal_spi.h
 * @brief SPI Hardware Abstraction Layer for MS4000
 *
 * Provides SPI communication interface for APA102/WS2801 LED controllers.
 * Abstracts SPI hardware configuration and data transfer.
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H

#include "hal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** APA102 LED protocol constants */
    #define APA102_START_FRAME_SIZE 4 ///< Start frame: 4 bytes of 0x00
    #define APA102_END_FRAME_SIZE 4   ///< End frame: 4 bytes of 0x00
    #define APA102_LED_FRAME_SIZE 4   ///< Per-LED: [brightness|B|G|R]
    #define APA102_BRIGHTNESS_MASK 0xE0 ///< Brightness bits [111xxxxx]
    #define APA102_BRIGHTNESS_MAX 0x1F  ///< Max brightness value (5 bits)

    /** SPI frequency configurations */
    #define HAL_SPI_FREQ_APA102 20000000 ///< 20 MHz for APA102
    #define HAL_SPI_FREQ_WS2801 500000   ///< 500 kHz for WS2801

    /**
     * @brief Initialize SPI bus for LED communication
     *
     * Uses hardware SPI on ESP8266 (fixed pins: MOSI=GPIO13, SCK=GPIO14).
     *
     * @param frequency SPI clock frequency in Hz
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_spi_init(uint32_t frequency);

    /**
     * @brief Write data buffer to SPI bus
     *
     * @param data Pointer to data buffer
     * @param length Number of bytes to write
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_spi_write(const uint8_t *data, uint16_t length);

    /**
     * @brief Set SPI clock frequency
     *
     * @param frequency New frequency in Hz
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_spi_set_frequency(uint32_t frequency);

#ifdef __cplusplus
}
#endif

#endif // HAL_SPI_H
