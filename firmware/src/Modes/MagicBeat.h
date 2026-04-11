
#include <WiFiClient.h>
#include <MQTT.h>

#include <WiFiUdp.h>

#define BEAT_MODE_USE_MQTT 1


class MagicBeatMode : public MagicShifterBaseMode {

private:

	float avgZ;
	int colorIndex;

	MS4_App_Beat &_beat = msGlobals.pbuf.apps.beat;

#ifdef BEAT_MODE_USE_MQTT
	WiFiClient	beatNet;
	MQTTClient	beatMQTT;
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
		extern MagicShifterSystem msSystem;
		msSystem.slog("incoming: " + topic + " - " + payload);
	}
#endif

	virtual void start() {
		if (!hasContext()) return;

		extern MagicShifterSystem msSystem;

		// hack: wizards work colleague
		_beat.color.R = 10;
		_beat.color.G = 255;
		_beat.color.B = 10;

		_beat.sensitivity = 2;
		_beat.mode = MS4_App_Beat_Mode_SIDE;


		if (context->getButtons().isButtonBPressed()) {
					_beat.sensitivity = 1;
		}


#ifdef BEAT_MODE_USE_MQTT
		String mqttName = msSystem.Settings.getUniqueSystemName();

		// TODO: replace with broker IP
	beatMQTT.begin("91.92.136.115", beatNet);
		context->getLogger().log(mqttName.c_str());

		while (!beatMQTT.connect(mqttName.c_str(), "public", "public")) {
			context->getLogger().log(".");
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
	const float avgF = 1./32.0;

	virtual bool step(void) {
		if (!hasContext()) return true;

		extern MagicShifterSystem msSystem;
		extern MagicShifterGlobals msGlobals;

		auto& buttons = context->getButtons();
		auto& leds = context->getLEDs();
		uint8_t brightness = context->getBrightness();

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
		}
		else if (_beat.mode == MS4_App_Beat_Mode_CENTER) {
			xPos += 7.5; //(MAX_LEDS - 1) / 2.0f;
		}

		leds.fillLEDs(0, 0, 0, brightness);

		if (_beat.mode == MS4_App_Beat_Mode_SIDE) {
			for (int i = 0; i < xPos; i++)
				leds.setLED((MAX_LEDS-1) - i,
					_beat.color.R,
					_beat.color.G,
					_beat.color.B,
					brightness);
		}
		else if (_beat.mode == MS4_App_Beat_Mode_CENTER) {
			int xPosInt = (int)(xPos);
			float xPosRem = 1 - (xPos - xPosInt);

			if (xPosInt >= 0 && xPosInt < MAX_LEDS) {
				leds.setLED(xPosInt,
					(int)(_beat.color.R * xPosRem),
					(int)(_beat.color.G * xPosRem),
					(int)(_beat.color.B * xPosRem),
					brightness);
			}

			xPosInt++;
			xPosRem = 1 - xPosRem;
			if (xPosInt >= 0 && xPosInt < MAX_LEDS) {
				leds.setLED(xPosInt,
					(int)(_beat.color.R * xPosRem),
					(int)(_beat.color.G * xPosRem),
					(int)(_beat.color.B * xPosRem),
					brightness);
			}
		}

		leds.updateLEDs();

		if (buttons.isButtonALongPressed() || buttons.isButtonBLongPressed()) {

			msSystem.msButtons.msBtnALongHit = false;
			msSystem.msButtons.msBtnBLongHit = false;

			if (_beat.mode == MS4_App_Beat_Mode_CENTER) {
				_beat.mode = MS4_App_Beat_Mode_SIDE;
			}
			else {
				_beat.mode = MS4_App_Beat_Mode_CENTER;
			}

		}

		if (buttons.isButtonAPressed()) {
			_beat.sensitivity = (_beat.sensitivity + 1) % 6;
			msSystem.msButtons.msBtnAHit = false;
		}

		if (buttons.isButtonBPressed()) {
			_beat.sensitivity = (_beat.sensitivity + 5) % 6;
			msSystem.msButtons.msBtnBHit = false;
		}

		if (buttons.isPowerButtonPressed()) {
			colorIndex++;
			if (colorIndex > 7) colorIndex = 1;
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
