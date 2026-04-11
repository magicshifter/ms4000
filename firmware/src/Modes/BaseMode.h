/**
 * @file BaseMode.h
 * @brief Base class for all MS4000 modes
 *
 * Provides interface for mode lifecycle (start/stop/step) and MIDI events.
 * Phase 2B: Added optional IModeContext support for dependency injection.
 */

#ifndef _MAGICSHIFTERBASEMODE_H
#define _MAGICSHIFTERBASEMODE_H

// Forward declaration for optional dependency injection
class IModeContext;

/**
 * @brief Base class for all MagicShifter modes
 *
 * A MagicShifter mode can be activated by the user through system-events
 * (buttons, shaking, etc.)
 *
 * Lifecycle:
 * - start() is called when the user selects the mode
 * - step() is called every frame while mode is active
 * - stop() is called when switching to another mode
 *
 * Phase 2B: Modes can optionally use IModeContext for hardware access
 * instead of directly accessing msSystem/msGlobals globals.
 */
class MagicShifterBaseMode
{
protected:
    /**
     * @brief Optional context for dependency injection
     *
     * Modes refactored to use IModeContext should access hardware through
     * this pointer. Legacy modes can continue using msSystem/msGlobals.
     *
     * Set via setContext() - nullptr by default for backward compatibility.
     */
    IModeContext *context = nullptr;

public:
    /**
     * @brief Unique name for the mode
     */
    String modeName = "BaseMode";

    /**
     * @brief Default constructor (backward compatible)
     */
    MagicShifterBaseMode() = default;

    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~MagicShifterBaseMode() = default;

    /**
     * @brief Set execution context (for dependency injection)
     *
     * @param ctx Pointer to IModeContext implementation
     */
    void setContext(IModeContext *ctx) { context = ctx; }

    /**
     * @brief Check if context is available
     * @return true if context has been set
     */
    bool hasContext() const { return context != nullptr; }

    /**
     * @brief Start the mode
     *
     * Called when mode is activated. Initialize mode state here.
     */
    virtual void start() {}

    /**
     * @brief Stop the mode
     *
     * Called when mode is deactivated. Clean up resources here.
     */
    virtual void stop() {}

    /**
     * @brief Execute one frame of the mode
     *
     * Called every frame while mode is active. Update LED display,
     * process input, and perform mode logic here.
     *
     * @return false (reserved for future use)
     */
    virtual bool step() { return false; }

    // ========================================================================
    // MIDI Event Handlers (optional, enabled with CONFIG_MIDI)
    // ========================================================================

#ifdef CONFIG_MIDI
    virtual void MIDI_Reset() {}
    virtual void MIDI_Start() {}
    virtual void MIDI_Stop() {}
    virtual void MIDI_Continue() {}
    virtual void MIDI_ActiveSensing() {}
    virtual void MIDI_Program_Change(byte channel, byte program) {}
    virtual void MIDI_Control_Change(byte channel, byte cc1, byte cc2) {}
    virtual void MIDI_Note_On(byte channel, byte note, byte velocity) {}
    virtual void MIDI_Note_Off(byte channel, byte note, byte velocity) {}
    virtual void MIDI_Aftertouch_Poly(byte channel, byte note, byte velocity) {}
    virtual void MIDI_Aftertouch_Channel(byte channel, byte program) {}
    virtual void MIDI_PitchBend(byte channel, byte bend) {}
    virtual void MIDI_SYSEX(const byte *data, uint16_t size) {}
    virtual void MIDI_TimeCodeQuarterFrame(byte data) {}
    virtual void MIDI_SongPosition(unsigned short beats) {}
    virtual void MIDI_SongSelect(byte songnumber) {}
    virtual void MIDI_TuneRequest() {}
    virtual void MIDI_Clock() {}
#endif
};

#endif
