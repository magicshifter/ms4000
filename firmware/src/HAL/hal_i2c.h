/**
 * @file hal_i2c.h
 * @brief I2C Hardware Abstraction Layer for MS4000
 *
 * Provides I2C communication interface for FXOS8700CQ sensor.
 * Replaces direct Wire library calls with named register constants.
 */

#ifndef HAL_I2C_H
#define HAL_I2C_H

#include "hal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** FXOS8700CQ I2C addresses */
    #define FXOS8700CQ_I2C_ADDRESS 0x1E ///< Magnetometer+accel version
    #define MMA8452Q_I2C_ADDRESS 0x1C   ///< Accel-only version (early protos)

    /** FXOS8700CQ WHO_AM_I values */
    #define FXOS8700CQ_WHO_AM_I_VALUE 0xC7 ///< Expected ID for FXOS8700CQ
    #define MMA8452Q_WHO_AM_I_VALUE 0x2A   ///< Expected ID for MMA8452Q

    /** FXOS8700CQ/MMA8452Q Register Map */
    #define FXOS_REG_STATUS 0x00       ///< Data status register
    #define FXOS_REG_OUT_X_MSB 0x01    ///< Accel X-axis MSB
    #define FXOS_REG_OUT_X_LSB 0x02    ///< Accel X-axis LSB
    #define FXOS_REG_OUT_Y_MSB 0x03    ///< Accel Y-axis MSB
    #define FXOS_REG_OUT_Y_LSB 0x04    ///< Accel Y-axis LSB
    #define FXOS_REG_OUT_Z_MSB 0x05    ///< Accel Z-axis MSB
    #define FXOS_REG_OUT_Z_LSB 0x06    ///< Accel Z-axis LSB
    #define FXOS_REG_MAG_STATUS 0x07   ///< Magnetometer status
    #define FXOS_REG_WHO_AM_I 0x0D     ///< Device ID register
    #define FXOS_REG_XYZ_DATA_CFG 0x0E ///< Accel range config
    #define FXOS_REG_CTRL_REG1 0x2A    ///< System control 1
    #define FXOS_REG_CTRL_REG2 0x2B    ///< System control 2 (soft reset)
    #define FXOS_REG_MAG_DATA_START 0x33 ///< Magnetometer data start

    /** Magnetometer Control Registers */
    #define FXOS_REG_M_CTRL_REG1 0x5B ///< Mag control 1 (hybrid mode)
    #define FXOS_REG_M_CTRL_REG2 0x5C ///< Mag control 2 (auto-increment)

    /** Magnetometer Vector Magnitude Detection Registers */
    #define FXOS_REG_M_VECM_CFG 0x69        ///< Vector magnitude config
    #define FXOS_REG_M_VECM_THS_LSB 0x6A    ///< Threshold LSB
    #define FXOS_REG_M_VECM_THS_MSB 0x6B    ///< Threshold MSB
    #define FXOS_REG_M_VECM_CNT 0x6C        ///< Sample count
    #define FXOS_REG_M_VECM_INITX_MSB 0x6D  ///< Initial X reference MSB
    #define FXOS_REG_M_VECM_INITX_LSB 0x6E  ///< Initial X reference LSB
    #define FXOS_REG_M_VECM_INITY_MSB 0x6F  ///< Initial Y reference MSB
    #define FXOS_REG_M_VECM_INITY_LSB 0x70  ///< Initial Y reference LSB
    #define FXOS_REG_M_VECM_INITZ_MSB 0x71  ///< Initial Z reference MSB
    #define FXOS_REG_M_VECM_INITZ_LSB 0x72  ///< Initial Z reference LSB

    /** Register bit definitions */
    #define FXOS_CTRL_REG1_ACTIVE 0x01 ///< Active mode enable bit
    #define FXOS_CTRL_REG2_RST 0x40    ///< Software reset bit

    /**
     * @brief Vector magnitude config value:
     * - Event latching enabled (m_vecm_ele=1)
     * - Use initial reference values (m_vecm_initm=1)
     * - Do not update reference (m_vecm_updm=1)
     * - Enable detection feature (m_vecm_en=1)
     */
    #define FXOS_M_VECM_CFG_INIT_MODE 0x7B

    /**
     * @brief Magnetometer control 1 value for hybrid mode:
     * - Hybrid mode enabled
     * - Oversampling ratio = 32
     */
    #define FXOS_M_CTRL_REG1_HYBRID 0x1F

    /**
     * @brief Magnetometer control 1 auto-calibration enable bit
     */
    #define FXOS_M_CTRL_REG1_ACAL 0x80

    /**
     * @brief Magnetometer control 2 value for hybrid mode:
     * - Hybrid auto-increment mode enabled
     */
    #define FXOS_M_CTRL_REG2_HYBRID 0x20

    /**
     * @brief Initialize I2C bus for sensor communication
     *
     * @param sda_pin GPIO pin for I2C data (SDA)
     * @param scl_pin GPIO pin for I2C clock (SCL)
     * @param frequency I2C clock frequency in Hz (max 500000 for ESP8266)
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_i2c_init(uint8_t sda_pin, uint8_t scl_pin, uint32_t frequency);

    /**
     * @brief Write a single byte to I2C device register
     *
     * @param device_addr 7-bit I2C device address
     * @param reg_addr Register address to write
     * @param data Byte value to write
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_i2c_write_register(uint8_t device_addr, uint8_t reg_addr, uint8_t data);

    /**
     * @brief Read a single byte from I2C device register
     *
     * @param device_addr 7-bit I2C device address
     * @param reg_addr Register address to read
     * @param data Pointer to store read byte
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_i2c_read_register(uint8_t device_addr, uint8_t reg_addr, uint8_t *data);

    /**
     * @brief Read multiple bytes from I2C device registers
     *
     * @param device_addr 7-bit I2C device address
     * @param reg_addr Starting register address
     * @param data Buffer to store read bytes
     * @param length Number of bytes to read
     * @return HAL_OK on success, error code otherwise
     */
    hal_status_t hal_i2c_read_registers(uint8_t device_addr,
                                        uint8_t reg_addr,
                                        uint8_t *data,
                                        uint8_t length);

#ifdef __cplusplus
}
#endif

#endif // HAL_I2C_H
