/**
 * @file ISensorInput.h
 * @brief Sensor Input Interface for MS4000
 *
 * Abstract interface for accelerometer and magnetometer sensor access.
 * Decouples modes from specific sensor hardware (FXOS8700CQ, MMA8452Q).
 *
 * Part of Phase 2B refactoring - Dependency Injection layer.
 */

#ifndef ISENSOR_INPUT_H
#define ISENSOR_INPUT_H

#include <stdint.h>

/**
 * @brief Abstract interface for sensor input
 *
 * Provides access to accelerometer and magnetometer readings.
 * Values are normalized and calibrated by the implementation.
 */
class ISensorInput
{
public:
    virtual ~ISensorInput() = default;

    /**
     * @brief Read accelerometer data
     *
     * @param destination Array of 3 floats to store [X, Y, Z] acceleration in g's
     */
    virtual void readAcceleration(float *destination) = 0;

    /**
     * @brief Read magnetometer data
     *
     * @param destination Array of 3 ints to store [X, Y, Z] magnetic field
     */
    virtual void readMagnetometer(int *destination) = 0;

    /**
     * @brief Get X-axis acceleration
     * @return X-axis acceleration in g's
     */
    virtual float getAccelX() = 0;

    /**
     * @brief Get Y-axis acceleration
     * @return Y-axis acceleration in g's
     */
    virtual float getAccelY() = 0;

    /**
     * @brief Get Z-axis acceleration
     * @return Z-axis acceleration in g's
     */
    virtual float getAccelZ() = 0;

    /**
     * @brief Check if sensor is available and responding
     * @return true if sensor is working
     */
    virtual bool isSensorAvailable() = 0;
};

#endif // ISENSOR_INPUT_H
