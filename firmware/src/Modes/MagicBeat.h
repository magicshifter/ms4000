
#include <MQTT.h>
#include <WiFiClient.h>

#include <WiFiUdp.h>

//#define BEAT_MODE_USE_MQTT 1

class MagicBeatMode : public MagicShifterBaseMode {

private:
  float avgZ;
  int colorIndex;

  MS4_App_Beat &_beat = msGlobals.pbuf.apps.beat;

#ifdef BEAT_MODE_USE_MQTT
  WiFiClient beatNet;
  MQTTClient beatMQTT;
#endif

public:
  MagicBeatMode() {
    modeName = "Beat";

    colorIndex = 2;

    msGlobals.pbuf.has_apps = 1;
    msGlobals.pbuf.apps.has_current = 1;
    msGlobals.pbuf.apps.has_beat = 1;

    // _beat = &msGlobals.pbuf.apps.beat;

    _beat.color.R = 10;
    _beat.color.G = 255;
    _beat.color.B = 10;

    _beat.sensitivity = 2;
    _beat.mode = MS4_App_Beat_Mode_SIDE;

    _beat.has_color = 1;
    _beat.has_mode = 1;
    _beat.has_sensitivity = 1;
  }

#ifdef BEAT_MODE_USE_MQTT
  static void mqttMessageReceived(String &topic, String &payload) {
    msSystem.slog("incoming: " + topic + " - " + payload);
  }
#endif

  virtual void start() {

    // hack: wizards work colleague
    _beat.color.R = 10;
    _beat.color.G = 255;
    _beat.color.B = 10;

    _beat.sensitivity = 2;
    _beat.mode = MS4_App_Beat_Mode_SIDE;

    if (msSystem.msButtons.msBtnBHit) {
      _beat.sensitivity = 1;
    }

#ifdef BEAT_MODE_USE_MQTT
    String mqttName = msSystem.Settings.getUniqueSystemName();

    // TODO: replace with broker IP
    beatMQTT.begin("91.92.136.115", beatNet);
    msSystem.slog(mqttName);

    while (!beatMQTT.connect(mqttName.c_str(), "public", "public")) {
      msSystem.slog(".");
      delay(500);
    }

    beatMQTT.onMessage(mqttMessageReceived);
    beatMQTT.subscribe("beat");

#endif
  }

  virtual void stop(void) {

#ifdef BEAT_MODE_USE_MQTT
    beatMQTT.unsubscribe("beat");
    beatMQTT.disconnect();
#endif
  }

  const int axis = 2;
  const float avgF = 1. / 32.0;

  virtual bool step(void) {
    float xPos;

    avgZ = avgZ * (1 - avgF) + avgF * msGlobals.ggAccel[axis];

    xPos = (msGlobals.ggAccel[axis] - avgZ);

    xPos *= 1024 >> _beat.sensitivity;

#if 0 
		printf( " subMode: %d" \
				" sensitivity: %d" \
				" colorIndex: %d" \
				" R: %d " \
				" G: %d " \
				" B: %d \n",
					_beat.mode,
					_beat.sensitivity,
					colorIndex,
					_beat.color.R,
					_beat.color.G,
					_beat.color.B);
#endif

    if (_beat.mode == MS4_App_Beat_Mode_SIDE) {
      if (xPos < 0)
        xPos = -xPos;

      xPos -= 5;
      if (xPos > MAX_LEDS)
        xPos = MAX_LEDS;
    } else if (_beat.mode == MS4_App_Beat_Mode_CENTER) {
      //xPos += 7.5;
      xPos += (MAX_LEDS - 1) / 2.0f;
    }

    msSystem.msLEDs.fillLEDs(0, 0, 0, msGlobals.ggBrightness);

    if (_beat.mode == MS4_App_Beat_Mode_SIDE) {
      for (int i = 0; i < xPos; i++)
        msSystem.msLEDs.setLED((MAX_LEDS - 1) - i, _beat.color.R, _beat.color.G,
                               _beat.color.B, msGlobals.ggBrightness);
    } else if (_beat.mode == MS4_App_Beat_Mode_CENTER) {
      int xPosInt = (int)(xPos);
      float xPosRem = 1 - (xPos - xPosInt);

      if (xPosInt >= 0 && xPosInt < MAX_LEDS) {
        msSystem.msLEDs.setLED(xPosInt, (int)(_beat.color.R * xPosRem),
                               (int)(_beat.color.G * xPosRem),
                               (int)(_beat.color.B * xPosRem),
                               msGlobals.ggBrightness);
      }

      xPosInt++;
      xPosRem = 1 - xPosRem;
      if (xPosInt >= 0 && xPosInt < MAX_LEDS) {
        msSystem.msLEDs.setLED(xPosInt, (int)(_beat.color.R * xPosRem),
                               (int)(_beat.color.G * xPosRem),
                               (int)(_beat.color.B * xPosRem),
                               msGlobals.ggBrightness);
      }
    }

    msSystem.msLEDs.updateLEDs();

    if ((msSystem.msButtons.msBtnALongHit) ||
        (msSystem.msButtons.msBtnBLongHit)) {

      msSystem.msButtons.msBtnALongHit = false;
      msSystem.msButtons.msBtnBLongHit = false;

      if (_beat.mode == MS4_App_Beat_Mode_CENTER) {
        _beat.mode = MS4_App_Beat_Mode_SIDE;
      } else {
        _beat.mode = MS4_App_Beat_Mode_CENTER;
      }
    }

    if (msSystem.msButtons.msBtnAHit) {
      _beat.sensitivity = (_beat.sensitivity + 1) % 6;
      msSystem.msButtons.msBtnAHit = false;
    }

    if (msSystem.msButtons.msBtnBHit) {
      _beat.sensitivity = (_beat.sensitivity + 5) % 6;
      msSystem.msButtons.msBtnBHit = false;
    }

    if (msSystem.msButtons.msBtnPwrHit) {
      colorIndex++;
      if (colorIndex > 7)
        colorIndex = 1;
      _beat.color.R = (colorIndex & 1) ? 255 : 0;
      _beat.color.G = (colorIndex & 2) ? 255 : 0;
      _beat.color.B = (colorIndex & 4) ? 255 : 0;
      msSystem.msButtons.msBtnPwrHit = false;
    }

#ifdef BEAT_MODE_USE_MQTT
    String beatMQTTMsg = " it ";
    beatMQTT.publish("beat", beatMQTTMsg);
#endif

    return true;
  }
};
