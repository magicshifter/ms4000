/**
 * @file IButtonInput.h
 * @brief Button Input Interface for MS4000
 *
 * Abstract interface for button input handling. Provides event-based
 * button access (single click, long press, double click) without
 * direct hardware dependencies.
 *
 * Part of Phase 2B refactoring - Dependency Injection layer.
 */

#ifndef IBUTTON_INPUT_H
#define IBUTTON_INPUT_H

/**
 * @brief Abstract interface for button input
 *
 * Provides button event detection (clicks, long presses, double clicks)
 * for buttons A, B, and Power. Implementations handle debouncing and
 * timing internally.
 */
class IButtonInput
{
public:
    virtual ~IButtonInput() = default;

    // ========================================================================
    // Button A Events
    // ========================================================================

    /**
     * @brief Check if button A was pressed (single click)
     * @return true if button A was clicked since last check
     */
    virtual bool isButtonAPressed() = 0;

    /**
     * @brief Check if button A was long-pressed
     * @return true if button A was held down for >750ms
     */
    virtual bool isButtonALongPressed() = 0;

    /**
     * @brief Check if button A was double-clicked
     * @return true if button A was clicked twice within 375ms
     */
    virtual bool isButtonADoublePressed() = 0;

    // ========================================================================
    // Button B Events
    // ========================================================================

    /**
     * @brief Check if button B was pressed (single click)
     * @return true if button B was clicked since last check
     */
    virtual bool isButtonBPressed() = 0;

    /**
     * @brief Check if button B was long-pressed
     * @return true if button B was held down for >750ms
     */
    virtual bool isButtonBLongPressed() = 0;

    /**
     * @brief Check if button B was double-clicked
     * @return true if button B was clicked twice within 375ms
     */
    virtual bool isButtonBDoublePressed() = 0;

    // ========================================================================
    // Power Button Events
    // ========================================================================

    /**
     * @brief Check if power button was pressed
     * @return true if power button was pressed
     */
    virtual bool isPowerButtonPressed() = 0;

    /**
     * @brief Check if power button was long-pressed
     * @return true if power button was held down for >750ms
     */
    virtual bool isPowerButtonLongPressed() = 0;

    // ========================================================================
    // Event Management
    // ========================================================================

    /**
     * @brief Clear all button event flags
     *
     * Resets all button hit/long-hit/double-hit flags.
     * Typically called after handling events to prepare for next frame.
     */
    virtual void clearAllEvents() = 0;

    /**
     * @brief Check if any button is currently active
     * @return true if any button is currently pressed or has pending events
     */
    virtual bool isAnyButtonActive() = 0;
};

#endif // IBUTTON_INPUT_H
