/**
 * @file IModeContext.h
 * @brief Mode Context Interface for MS4000
 *
 * Unified interface providing modes with access to all hardware, services,
 * and configuration. Single injection point that replaces direct access to
 * msSystem and msGlobals globals.
 *
 * Part of Phase 2B refactoring - Dependency Injection layer.
 */

#ifndef IMODE_CONTEXT_H
#define IMODE_CONTEXT_H

#include "ILEDController.h"
#include "IButtonInput.h"
#include "ISensorInput.h"
#include "IModeConfig.h"
#include "ILogger.h"
#include <stdint.h>

/**
 * @brief Abstract interface for mode execution context
 *
 * Provides unified access to all hardware and services that modes need.
 * Replaces direct dependencies on msSystem and msGlobals, enabling:
 * - Unit testing with mock implementations
 * - Runtime hardware configuration
 * - Better encapsulation and maintainability
 */
class IModeContext
{
public:
    virtual ~IModeContext() = default;

    // ========================================================================
    // Hardware Access
    // ========================================================================

    /**
     * @brief Get LED controller interface
     * @return Reference to LED controller
     */
    virtual ILEDController &getLEDs() = 0;

    /**
     * @brief Get button input interface
     * @return Reference to button input handler
     */
    virtual IButtonInput &getButtons() = 0;

    /**
     * @brief Get sensor input interface
     * @return Reference to sensor input handler
     */
    virtual ISensorInput &getSensor() = 0;

    // ========================================================================
    // Configuration & State
    // ========================================================================

    /**
     * @brief Get mode-specific configuration
     * @return Reference to configuration interface
     */
    virtual IModeConfig &getConfig() = 0;

    /**
     * @brief Get global brightness setting
     * @return Brightness level (0-31)
     */
    virtual uint8_t getBrightness() const = 0;

    /**
     * @brief Set global brightness setting
     * @param brightness Brightness level (0-31)
     */
    virtual void setBrightness(uint8_t brightness) = 0;

    /**
     * @brief Get current time in microseconds
     * @return Current microsecond counter
     */
    virtual uint32_t getCurrentMicros() const = 0;

    /**
     * @brief Get current time in milliseconds
     * @return Current millisecond counter
     */
    virtual uint32_t getCurrentMillis() const = 0;

    /**
     * @brief Get elapsed time since last frame
     * @return Delta time in microseconds
     */
    virtual uint32_t getDeltaMicros() const = 0;

    // ========================================================================
    // Services
    // ========================================================================

    /**
     * @brief Get logger interface
     * @return Reference to logger
     */
    virtual ILogger &getLogger() = 0;

    /**
     * @brief Request mode change
     *
     * @param modeIndex Index of mode to switch to
     * @return true if mode change was initiated
     */
    virtual bool requestModeChange(int modeIndex) = 0;
};

#endif // IMODE_CONTEXT_H
