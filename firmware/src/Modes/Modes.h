/**
 * @file Modes.h
 * @brief Mode instances and context initialization
 *
 * Instantiates all mode objects as globals and provides context setup.
 * Phase 2C: Added ProductionModeContext for dependency injection.
 */

#ifndef _MODES_H
#define _MODES_H

#define NUM_MS_MODES 6

// Include base mode class and interfaces before mode headers
#include "BaseMode.h"
#include "../Interfaces/IModeContext.h"

// Phase 2C: Forward declare context (defined after msSystem/msGlobals exist)
class ProductionModeContext;
extern ProductionModeContext *g_modeContext;

/**
 * @brief Initialize mode context and wire to all modes
 *
 * Must be called after msSystem and msGlobals are initialized.
 * Sets context on all mode instances to enable dependency injection.
 */
void initializeModeContext();

// rudimentary mode for displaying an msImage with POV
#include "UI/ShakeSync.h"

// OSC = Open Sound Control.  Can be used to send magnetometer/sensor data, for example
#ifdef CONFIG_ENABLE_OSC
#include "OSCMessage.h"
#endif // CONFIG_ENABLE_OSC

// MIDI modes and features can be configured, i.e. MIDIShifter project
#ifdef CONFIG_ENABLE_MIDI
#include "MIDI/envelope.h"
#include "MIDI/MIDIArpeggiator.h"
MIDIArpeggiatorMode msMIDIArpeggiator;
#include "MIDI/MIDISequence8.h"
MIDISequence8Mode msMIDISequencer;
#endif // CONFIG_ENABLE_MIDI

#include "MagicPOV.h"
#include "SystemText.h"
#include "MagicMagnet.h"
#include "BouncingBall.h"
#include "MagicShake.h"
#include "MagicLight.h"
#include "MagicRemote.h"
#include "MagicBeat.h"
#include "MagicCountdown.h"
#include "MagicUpdate.h"
#include "MagicConway.h"
#include "MagicBike.h"
#include "MagicMeasureMQTT.h"

#include "PowerEmergency.h"

#include "ModeSelector.h"

PowerEmergencyMode msPowerEmergencyMode;
SystemTextMode msSysText;

ModeSelectorMode msModeSelector;

MagicShakeMode msMagicShake;
MagicMagnetMode msMagicMagnet;
MagicLightMode msMagicLight;
MagicRemoteMode msMagicRemote;
MagicBeatMode msMagicBeat;
MagicUpdateMode msMagicUpdate;
MagicBikeMode msMagicBike;
MagicMeasureMQTTMode msMagicMeasureMQTT;

//MagicConwayMode msMagicConway;
//MagicCountdownMode msMagicCountdown;

// Phase 2C: Mode context initialization (inline to avoid separate compilation unit)
#include "../Interfaces/ProductionModeContext.h"

// Global mode context instance
ProductionModeContext *g_modeContext = nullptr;

/**
 * @brief Initialize mode context and set on all mode instances
 *
 * Creates ProductionModeContext wrapping msSystem and msGlobals,
 * then calls setContext() on all instantiated modes.
 *
 * Must be called after msSystem and msGlobals are initialized,
 * typically in MS3KOS::setup().
 */
inline void initializeModeContext()
{
    // Create production context wrapping globals
    static ProductionModeContext context(msSystem, msGlobals);
    g_modeContext = &context;

    // Set context on all mode instances
    msPowerEmergencyMode.setContext(g_modeContext);
    msSysText.setContext(g_modeContext);
    msModeSelector.setContext(g_modeContext);
    msMagicShake.setContext(g_modeContext);
    msMagicMagnet.setContext(g_modeContext);
    msMagicLight.setContext(g_modeContext);
    msMagicRemote.setContext(g_modeContext);
    msMagicBeat.setContext(g_modeContext);
    msMagicUpdate.setContext(g_modeContext);
    msMagicBike.setContext(g_modeContext);
    msMagicMeasureMQTT.setContext(g_modeContext);

#ifdef CONFIG_ENABLE_MIDI
    msMIDIArpeggiator.setContext(g_modeContext);
    msMIDISequencer.setContext(g_modeContext);
#endif
}

#endif
