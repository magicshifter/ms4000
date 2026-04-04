/**
 * @file BoardConfig.h
 * @brief Centralized MS4000 board configuration
 *
 * Single source of truth for all hardware pin assignments,
 * I2C/SPI parameters, and board-specific constants.
 * Replaces scattered definitions from msConfig.h, Sensor.h,
 * LEDHardware.h, and Buttons.h.
 */

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <stdint.h>

namespace MS4000
{
    namespace BoardConfig
    {

        // ====================================================================
        // Hardware Identification
        // ====================================================================

        /** Hardware configuration variants */
        enum class HardwareID : uint8_t
        {
            MS3000_FXOS8700CQ = 1, ///< MS3000 with magnetometer+accel
            MS3000_MMA8452Q = 2,   ///< MS3000 with accel only (early prototypes)
            RING = 3,              ///< Ring variant with attached strip
            SABRE = 4              ///< Sabre variant with attached strip
        };

        /** LED controller types */
        enum class LEDType : uint8_t
        {
            APA102 = 1, ///< APA102 (default for MS3000)
            WS2801 = 2  ///< WS2801 (used in RING/SABRE)
        };

        // ====================================================================
        // GPIO Pin Assignments
        // ====================================================================

        namespace Pins
        {
            // I2C Bus (Sensor)
            constexpr uint8_t I2C_SDA = 5; ///< GPIO5 (D1 on NodeMCU) - blue wire
            constexpr uint8_t I2C_SCL = 4; ///< GPIO4 (D2 on NodeMCU) - purple wire

            // SPI Bus (LEDs) - Hardware SPI, pins are fixed on ESP8266
            constexpr uint8_t SPI_MOSI = 13; ///< GPIO13 (D7) - LED data
            constexpr uint8_t SPI_SCK = 14;  ///< GPIO14 (D5) - LED clock

            // LED Control
            constexpr uint8_t LED_ENABLE = 15; ///< GPIO15 (D8) - LED power enable

            // User Buttons
            constexpr uint8_t BUTTON_A = 12; ///< GPIO12 (D6) - doubles as bootloader button
            constexpr uint8_t BUTTON_B = 0;  ///< GPIO0 (D3) - boot mode select

            // Power Management
            constexpr uint8_t POWER_MGMT = 16; ///< GPIO16 (D0) - power management
        } // namespace Pins

        // ====================================================================
        // I2C Configuration
        // ====================================================================

        namespace I2C
        {
            constexpr uint32_t FREQUENCY = 500000; ///< 500 kHz (ESP8266 max safe)
            constexpr uint32_t TIMEOUT_MS = 100;   ///< Transaction timeout
        } // namespace I2C

        // ====================================================================
        // SPI Configuration
        // ====================================================================

        namespace SPI
        {
            constexpr uint32_t FREQ_APA102 = 20000000; ///< 20 MHz for APA102
            constexpr uint32_t FREQ_WS2801 = 500000;   ///< 500 kHz for WS2801
        } // namespace SPI

        // ====================================================================
        // Button Configuration
        // ====================================================================

        namespace Buttons
        {
            constexpr uint32_t MIN_CLICK_TIME_US = 10000;        ///< 10ms debounce
            constexpr uint32_t MIN_LONG_CLICK_TIME_US = 750000;  ///< 750ms for long press
            constexpr uint32_t MAX_DOUBLE_CLICK_TIME_MS = 375;   ///< 375ms between clicks
            constexpr uint16_t POWER_BUTTON_ADC_THRESHOLD = 970; ///< ADC threshold for power button
        } // namespace Buttons

        // ====================================================================
        // Sensor Configuration
        // ====================================================================

        namespace Sensor
        {
            /** Accelerometer full-scale range (+/- g) */
            enum class AccelRange : uint8_t
            {
                RANGE_2G = 2,
                RANGE_4G = 4,
                RANGE_8G = 8
            };

            constexpr AccelRange ACCEL_FULL_SCALE = AccelRange::RANGE_8G;

            /** Axis indices for accelerometer data */
            constexpr uint8_t AXIS_X = 0;
            constexpr uint8_t AXIS_Y = 1;
            constexpr uint8_t AXIS_Z = 2;

            /** Magnetometer configuration */
            constexpr uint16_t MAG_THRESHOLD_UT = 1000; ///< Threshold in microTeslas (1000 counts = 100.0uT)
            constexpr uint8_t MAG_SAMPLE_COUNT = 1;     ///< Sample count (1 * 20ms = 20ms in hybrid mode)
        } // namespace Sensor

        // ====================================================================
        // Power Management
        // ====================================================================

        namespace Power
        {
            constexpr uint32_t TIMEOUT_HIGH_POWER_MS = 0;               ///< No timeout in high power mode
            constexpr uint32_t TIMEOUT_LOW_POWER_MS = 10 * 60 * 1000;   ///< 10 minutes
            constexpr uint8_t DEFAULT_BRIGHTNESS = 2;                   ///< Default LED brightness (0-31)
        } // namespace Power

        // ====================================================================
        // System Configuration
        // ====================================================================

        namespace System
        {
            constexpr const char *VERSION = "4.0.0";
            constexpr const char *AP_NAME = "MS4000";

            constexpr uint8_t MAX_FILENAME_LENGTH = 32;
            constexpr uint8_t MAX_TEXT_LENGTH = 16;

            constexpr uint32_t POV_TIME_MICROSECONDS = 100;
            constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 3 * 10 * 1000; ///< 3 * 10 seconds

            constexpr const char *DEFAULT_SHAKE_IMAGE = "heart.magicBitmap";

            /** System fault codes */
            enum class FaultCode : uint8_t
            {
                NONE = 0,
                NO_ACCELEROMETER = 0xF1,
                VERY_LOW_POWER = 0xF2
            };
        } // namespace System

    } // namespace BoardConfig
} // namespace MS4000

#endif // BOARD_CONFIG_H
