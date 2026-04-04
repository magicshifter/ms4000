/**
 * @file ProductionModeContext.h
 * @brief Production Implementation of IModeContext
 *
 * Wraps msSystem and msGlobals to provide IModeContext interface.
 * This adapter allows existing code to work through the new interface
 * while maintaining backward compatibility.
 *
 * Part of Phase 2B refactoring - Dependency Injection layer.
 */

#ifndef PRODUCTION_MODE_CONTEXT_H
#define PRODUCTION_MODE_CONTEXT_H

#include "IModeContext.h"

// Forward declarations to avoid circular dependencies
class MagicShifterSystem;
class MagicShifterGlobals;

/**
 * @brief Production implementation adapters
 *
 * These classes wrap existing hardware drivers to implement
 * the new interface layer.
 */

/**
 * @brief LED Controller adapter for MagicShifterLEDs
 */
class LEDControllerAdapter : public ILEDController
{
private:
    class MagicShifterLEDs &leds;

public:
    LEDControllerAdapter(class MagicShifterLEDs &ledsRef);

    void setLED(int index, uint8_t r, uint8_t g, uint8_t b,
               uint8_t brightness) override;
    void setLED(int index, uint8_t r, uint8_t g, uint8_t b) override;
    void fillLEDs(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) override;
    void updateLEDs() override;
    void fastClear() override;
    void saveBuffer(uint8_t *buffer) override;
    void loadBuffer(uint8_t *buffer) override;
    void setAllChannel(int channel, uint8_t value) override;
    void setChannel(int index, int channel, uint8_t value) override;
};

/**
 * @brief Button Input adapter for MagicShifterButtons
 */
class ButtonInputAdapter : public IButtonInput
{
private:
    class MagicShifterButtons &buttons;

public:
    ButtonInputAdapter(class MagicShifterButtons &buttonsRef);

    bool isButtonAPressed() override;
    bool isButtonALongPressed() override;
    bool isButtonADoublePressed() override;
    bool isButtonBPressed() override;
    bool isButtonBLongPressed() override;
    bool isButtonBDoublePressed() override;
    bool isPowerButtonPressed() override;
    bool isPowerButtonLongPressed() override;
    void clearAllEvents() override;
    bool isAnyButtonActive() override;
};

/**
 * @brief Sensor Input adapter for MagicShifterAccelerometer
 */
class SensorInputAdapter : public ISensorInput
{
private:
    MagicShifterGlobals &globals;

public:
    SensorInputAdapter(MagicShifterGlobals &globalsRef);

    void readAcceleration(float *destination) override;
    void readMagnetometer(int *destination) override;
    float getAccelX() override;
    float getAccelY() override;
    float getAccelZ() override;
    bool isSensorAvailable() override;
};

/**
 * @brief Simple logger adapter for msSystem.slog()
 */
class SystemLoggerAdapter : public ILogger
{
private:
    MagicShifterSystem &system;

public:
    SystemLoggerAdapter(MagicShifterSystem &systemRef);

    void log(const char *message) override;
    void logln(const char *message) override;
    void error(const char *message) override;
    void warning(const char *message) override;
    void debug(const char *message) override;
};

/**
 * @brief Stub config adapter - returns defaults for now
 *
 * TODO: Implement proper protobuf wrapper in future
 */
class StubConfigAdapter : public IModeConfig
{
public:
    int getInt(const char *key, int defaultValue = 0) override
    {
        return defaultValue;
    }
    float getFloat(const char *key, float defaultValue = 0.0f) override
    {
        return defaultValue;
    }
    bool getBool(const char *key, bool defaultValue = false) override
    {
        return defaultValue;
    }
    bool setInt(const char *key, int value) override { return false; }
    bool setFloat(const char *key, float value) override { return false; }
    bool setBool(const char *key, bool value) override { return false; }
    bool save() override { return false; }
    bool load() override { return false; }
};

/**
 * @brief Production Mode Context
 *
 * Implements IModeContext by wrapping msSystem and msGlobals.
 * This provides the new interface while maintaining full compatibility
 * with existing code.
 */
class ProductionModeContext : public IModeContext
{
private:
    MagicShifterSystem &system;
    MagicShifterGlobals &globals;

    // Adapters
    LEDControllerAdapter ledAdapter;
    ButtonInputAdapter buttonAdapter;
    SensorInputAdapter sensorAdapter;
    SystemLoggerAdapter loggerAdapter;
    StubConfigAdapter configAdapter;

public:
    /**
     * @brief Construct production context with system references
     *
     * @param sysRef Reference to msSystem global
     * @param globalsRef Reference to msGlobals global
     */
    ProductionModeContext(MagicShifterSystem &sysRef, MagicShifterGlobals &globalsRef);

    // Hardware access
    ILEDController &getLEDs() override { return ledAdapter; }
    IButtonInput &getButtons() override { return buttonAdapter; }
    ISensorInput &getSensor() override { return sensorAdapter; }

    // Configuration & state
    IModeConfig &getConfig() override { return configAdapter; }
    uint8_t getBrightness() const override;
    void setBrightness(uint8_t brightness) override;
    uint32_t getCurrentMicros() const override;
    uint32_t getCurrentMillis() const override;
    uint32_t getDeltaMicros() const override;

    // Services
    ILogger &getLogger() override { return loggerAdapter; }
    bool requestModeChange(int modeIndex) override;
};

// ============================================================================
// Inline Implementations
// ============================================================================

// LED Controller
inline LEDControllerAdapter::LEDControllerAdapter(class MagicShifterLEDs &ledsRef) : leds(ledsRef) {}

inline void LEDControllerAdapter::setLED(int index, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    leds.setLED(index, r, g, b, brightness);
}

inline void LEDControllerAdapter::setLED(int index, uint8_t r, uint8_t g, uint8_t b) {
    leds.setLED(index, r, g, b);
}

inline void LEDControllerAdapter::fillLEDs(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    leds.fillLEDs(r, g, b, brightness);
}

inline void LEDControllerAdapter::updateLEDs() {
    leds.updateLEDs();
}

inline void LEDControllerAdapter::fastClear() {
    leds.fastClear();
}

inline void LEDControllerAdapter::saveBuffer(uint8_t *buffer) {
    leds.saveBuffer(buffer);
}

inline void LEDControllerAdapter::loadBuffer(uint8_t *buffer) {
    leds.loadBuffer(buffer);
}

inline void LEDControllerAdapter::setAllChannel(int channel, uint8_t value) {
    leds.setAllChannel(channel, value);
}

inline void LEDControllerAdapter::setChannel(int index, int channel, uint8_t value) {
    leds.setChannel(index, channel, value);
}

// Button Input
inline ButtonInputAdapter::ButtonInputAdapter(class MagicShifterButtons &buttonsRef) : buttons(buttonsRef) {}

inline bool ButtonInputAdapter::isButtonAPressed() { return buttons.msBtnAHit; }
inline bool ButtonInputAdapter::isButtonALongPressed() { return buttons.msBtnALongHit; }
inline bool ButtonInputAdapter::isButtonADoublePressed() { return buttons.msBtnADoubleHit; }
inline bool ButtonInputAdapter::isButtonBPressed() { return buttons.msBtnBHit; }
inline bool ButtonInputAdapter::isButtonBLongPressed() { return buttons.msBtnBLongHit; }
inline bool ButtonInputAdapter::isButtonBDoublePressed() { return buttons.msBtnBDoubleHit; }
inline bool ButtonInputAdapter::isPowerButtonPressed() { return buttons.msBtnPwrHit; }
inline bool ButtonInputAdapter::isPowerButtonLongPressed() { return buttons.msBtnPwrLongHit; }
inline void ButtonInputAdapter::clearAllEvents() { buttons.resetButtons(); }
inline bool ButtonInputAdapter::isAnyButtonActive() { return buttons.msBtnActive; }

// Sensor Input
inline SensorInputAdapter::SensorInputAdapter(MagicShifterGlobals &globalsRef) : globals(globalsRef) {}

inline void SensorInputAdapter::readAcceleration(float *destination) {
    destination[0] = globals.ggAccel[0];
    destination[1] = globals.ggAccel[1];
    destination[2] = globals.ggAccel[2];
}

inline void SensorInputAdapter::readMagnetometer(int *destination) {
    destination[0] = globals.ggMagnet[0];
    destination[1] = globals.ggMagnet[1];
    destination[2] = globals.ggMagnet[2];
}

inline float SensorInputAdapter::getAccelX() { return globals.ggAccel[0]; }
inline float SensorInputAdapter::getAccelY() { return globals.ggAccel[1]; }
inline float SensorInputAdapter::getAccelZ() { return globals.ggAccel[2]; }
inline bool SensorInputAdapter::isSensorAvailable() { return globals.ggFault != FAULT_NO_ACCELEROMETER; }

// Logger
inline SystemLoggerAdapter::SystemLoggerAdapter(MagicShifterSystem &systemRef) : system(systemRef) {}

inline void SystemLoggerAdapter::log(const char *message) { system.slog(message); }
inline void SystemLoggerAdapter::logln(const char *message) { system.slogln(message); }
inline void SystemLoggerAdapter::error(const char *message) { system.slogln(String("ERROR: ") + String(message)); }
inline void SystemLoggerAdapter::warning(const char *message) { system.slogln(String("WARNING: ") + String(message)); }
inline void SystemLoggerAdapter::debug(const char *message) { system.slogln(String("DEBUG: ") + String(message)); }

// Production Mode Context
inline ProductionModeContext::ProductionModeContext(MagicShifterSystem &sysRef, MagicShifterGlobals &globalsRef)
    : system(sysRef), globals(globalsRef),
      ledAdapter(sysRef.msLEDs),
      buttonAdapter(sysRef.msButtons),
      sensorAdapter(globalsRef),
      loggerAdapter(sysRef)
{}

inline uint8_t ProductionModeContext::getBrightness() const { return globals.ggBrightness; }
inline void ProductionModeContext::setBrightness(uint8_t brightness) { globals.ggBrightness = brightness; }
inline uint32_t ProductionModeContext::getCurrentMicros() const { return globals.ggCurrentMicros; }
inline uint32_t ProductionModeContext::getCurrentMillis() const { return globals.ggCurrentMillis; }
inline uint32_t ProductionModeContext::getDeltaMicros() const { return globals.ggCurrentMicros - globals.ggLastMicros; }
inline bool ProductionModeContext::requestModeChange(int modeIndex) { return false; }  // Not implemented

#endif // PRODUCTION_MODE_CONTEXT_H
