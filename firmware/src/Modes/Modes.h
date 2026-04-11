#ifndef _MODES_H
#define _MODES_H

#define NUM_MS_MODES 6

// rudimentary mode for displaying an msImage with POV
#include "UI/ShakeSync.h"

// OSC = Open Sound Control.  Can be used to send magnetometer/sensor data, for
// example
#ifdef CONFIG_ENABLE_OSC
#include "OSCMessage.h"
#endif // CONFIG_ENABLE_OSC

// MIDI modes and features can be configured, i.e. MIDIShifter project
#ifdef CONFIG_ENABLE_MIDI
#include "MIDI/MIDIArpeggiator.h"
#include "MIDI/envelope.h"
MIDIArpeggiatorMode msMIDIArpeggiator;
#include "MIDI/MIDISequence8.h"
MIDISequence8Mode msMIDISequencer;
#endif // CONFIG_ENABLE_MIDI

#include "BouncingBall.h"
#include "MagicBeat.h"
#include "MagicBike.h"
// #include "MagicConway.h"
#include "MagicCountdown.h"
#include "MagicLight.h"
#include "MagicMagnet.h"
#include "MagicMeasureMQTT.h"
#include "MagicPOV.h"
#include "MagicRemote.h"
#include "MagicShake.h"
#include "MagicUpdate.h"
#include "SystemText.h"

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

// MagicConwayMode msMagicConway;
// MagicCountdownMode msMagicCountdown;

#endif
