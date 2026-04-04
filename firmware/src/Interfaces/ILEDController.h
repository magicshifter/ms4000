/**
 * @file ILEDController.h
 * @brief LED Controller Interface for MS4000
 *
 * Abstract interface for LED control operations. Allows modes to control LEDs
 * without direct dependency on hardware implementation. Enables testing with
 * mock implementations.
 *
 * Part of Phase 2B refactoring - Dependency Injection layer.
 */

#ifndef ILED_CONTROLLER_H
#define ILED_CONTROLLER_H

#include <stdint.h>

/**
 * @brief Abstract interface for LED control
 *
 * Provides methods for setting individual LEDs, filling all LEDs,
 * and updating the hardware. Implementations wrap hardware-specific
 * LED controllers (APA102, WS2801, etc.)
 */
class ILEDController
{
public:
    virtual ~ILEDController() = default;

    /**
     * @brief Set color and brightness of a single LED
     *
     * @param index LED index (0 to MAX_LEDS-1)
     * @param r Red value (0-255)
     * @param g Green value (0-255)
     * @param b Blue value (0-255)
     * @param brightness Brightness level (0-31 for APA102)
     */
    virtual void setLED(int index, uint8_t r, uint8_t g, uint8_t b,
                       uint8_t brightness) = 0;

    /**
     * @brief Fill all LEDs with the same color
     *
     * @param r Red value (0-255)
     * @param g Green value (0-255)
     * @param b Blue value (0-255)
     * @param brightness Brightness level (0-31)
     */
    virtual void fillLEDs(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) = 0;

    /**
     * @brief Update LED hardware with buffered data
     *
     * Flushes the LED buffer to the hardware via SPI.
     * Call this after setting LED values to make changes visible.
     */
    virtual void updateLEDs() = 0;

    /**
     * @brief Quickly clear all LEDs (turn off)
     *
     * Optimized method to turn off all LEDs without setting each individually.
     */
    virtual void fastClear() = 0;

    /**
     * @brief Save current LED buffer state
     *
     * @param buffer Destination buffer to save LED state
     */
    virtual void saveBuffer(uint8_t *buffer) = 0;

    /**
     * @brief Load LED buffer state
     *
     * @param buffer Source buffer to load LED state from
     */
    virtual void loadBuffer(uint8_t *buffer) = 0;

    /**
     * @brief Set LED color with default brightness
     *
     * Convenience overload that uses the global brightness setting.
     *
     * @param index LED index
     * @param r Red value
     * @param g Green value
     * @param b Blue value
     */
    virtual void setLED(int index, uint8_t r, uint8_t g, uint8_t b) = 0;

    /**
     * @brief Set all LEDs to same value for a specific color channel
     *
     * Used for RGB scanner effects.
     *
     * @param channel Color channel (0=R, 1=G, 2=B)
     * @param value Value for that channel (0-255)
     */
    virtual void setAllChannel(int channel, uint8_t value) = 0;

    /**
     * @brief Set a specific channel of a specific LED
     *
     * Used for RGB scanner effects.
     *
     * @param index LED index
     * @param channel Color channel (0=R, 1=G, 2=B)
     * @param value Value for that channel (0-255)
     */
    virtual void setChannel(int index, int channel, uint8_t value) = 0;
};

#endif // ILED_CONTROLLER_H
