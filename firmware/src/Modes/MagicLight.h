#ifndef _MAGICLIGHT_MODE_H
#define _MAGICLIGHT_MODE_H

//
// render some magic lights
//

class MagicLightMode : public MagicShifterBaseMode {

  private:
	int frame = 0;
	int xx = 0;
	int pDelay = 230;

	bool dir = false;

	int d = 10;
	uint8_t lookup[6][3] = { {0, 1, 2}, {1, 0, 2}, {2, 0, 1}, {0, 2, 1}, {1, 2, 0}, {2, 1, 0} };
	uint8_t lookupindex = 0;
	bool firstRun = false;
	int centerMode = 0;

	MS4_App_Light &_light = msGlobals.pbuf.apps.light;

public:

	MagicLightMode() {
		_light.colorIndex = 6;
		_light.triggerSpeed = 0; // milliseconds

	 	modeName = "Light";
	}

	// depending on button-presses, let the user select
	// sub-mode options
    bool lightSubModeSelector() {
		if (!hasContext()) return false;

		int old_light_mode = _light.mode;
		int new_light_mode = old_light_mode;
		int trigger_time = _light.triggerSpeed;

		if (firstRun)
			firstRun = false;
		else {
			// Note: Requires direct msSystem access for now (step() is complex)
			extern MagicShifterSystem msSystem;
			msSystem.step();
		}

		auto& buttons = context->getButtons();
		auto& leds = context->getLEDs();
		auto& logger = context->getLogger();
		uint8_t brightness = context->getBrightness();

		if (buttons.isPowerButtonPressed()) {
			centerMode++;
			// Note: Button flag clearing needs direct access until API is enhanced
			extern MagicShifterSystem msSystem;
			msSystem.msButtons.msBtnPwrHit = false;
		} else {
			trigger_time++;
		}

		if (buttons.isButtonAPressed()) {
			new_light_mode--;
			extern MagicShifterSystem msSystem;
			msSystem.msButtons.msBtnAHit = false;
		}
		if (buttons.isButtonBPressed()) {
			new_light_mode++;
			extern MagicShifterSystem msSystem;
			msSystem.msButtons.msBtnBHit = false;
		}


		if (new_light_mode < _MS4_App_Light_Mode_MIN)
			new_light_mode = _MS4_App_Light_Mode_MAX;

		if (new_light_mode > _MS4_App_Light_Mode_MAX)
			new_light_mode = _MS4_App_Light_Mode_MIN;


		if (new_light_mode != old_light_mode) {

			_light.mode = (MS4_App_Light_Mode)new_light_mode;

			logger.log("_light.mode: ");
			logger.logln(String(_light.mode).c_str());

			logger.log("_light.triggerSpeed: ");
			logger.logln(String(_light.triggerSpeed).c_str());

			logger.log("_light.trigger_time: ");
			logger.logln(String(trigger_time).c_str());

			leds.fillLEDs(255, 255, 255, brightness);
			leds.updateLEDs();
			delay(10);
			leds.fillLEDs(0, 0, 0, brightness);
			leds.updateLEDs();
		}

		// .. propagate the menu mode, in case its activited..
		extern MagicShifterSystem msSystem;
		return msSystem.modeMenuActivated;
	}

	void start() {
	}

	void stop(void) {
	}

	bool step() {
		if (!hasContext()) return true;

		frame++;
		firstRun = true;
		int start, end;

		auto& leds = context->getLEDs();
		auto& logger = context->getLogger();
		uint8_t brightness = context->getBrightness();

		lightSubModeSelector();

		// rainbow
		if (_light.mode == MS4_App_Light_Mode_RAINBOW) {
			if (frame % pDelay == 0) {
				leds.fillLEDs(0, 0, 0, brightness);
				leds.setLED((xx + 0 * 3) & MAX_LEDS, 255, 0, 0, brightness);

				leds.setLED((xx + 1 * 3) & MAX_LEDS, 255, 255, 0, brightness);
				leds.setLED((xx + 2 * 3) & MAX_LEDS, 0, 255, 0, brightness);

				leds.setLED((xx + 3 * 3) & MAX_LEDS, 0, 255, 255, brightness);
				leds.setLED((xx + 4 * 3) & MAX_LEDS, 0, 0, 255, brightness);

				xx++;
				leds.updateLEDs();
			}

			if (centerMode > 0) {
				centerMode--;
				pDelay += 1 + pDelay / 2;
				/*if (pDelay > 250 && pDelay < 500) {
					pDelay = 1000;
				} else */
				if (pDelay >= 5000) {
					pDelay = 1;
				}
			}
		}

		// normal
		if (_light.mode == MS4_App_Light_Mode_NORMAL) {
			int r=0,g=0,b=0;

			int ii = _light.colorIndex+1;

			if (ii & 1) r = 255;
			if (ii & 2) g = 255;
			if (ii & 4) b = 255;

			leds.fillLEDs(r, g, b, brightness);
			leds.updateLEDs();

			if (centerMode > 0) {
				centerMode--;
				_light.colorIndex = (_light.colorIndex + 1) % 7;
			}
		}

		if (_light.mode == MS4_App_Light_Mode_SCANNER_BW) {

			if (centerMode) {
				centerMode = 0;
			} else {
				centerMode = 1;
			}

			logger.log("centerMode: ");
			logger.logln(String(centerMode).c_str());

			if (dir)
			{
				start = 0;
				end = MAX_LEDS - 1;
			}
			else
			{
				start = MAX_LEDS - 1;
				end = 0;
			}
			startToEndZigZag(start, end, 1, 255, 255, 255);
			dir = (dir + 1) % 2;

		}

		// scanner
		if (_light.mode == MS4_App_Light_Mode_SCANNER_RGB) {
			if (dir)
			{
				start = 0;
				end = MAX_LEDS - 1;
			}
			else
			{
				start = MAX_LEDS - 1;
				end = 0;
			}

			if (centerMode > 0) {
				centerMode--;

				if (_light.mode == MS4_App_Light_Mode_SCANNER_RGB)
				{
					for (int index = 0; index < 3; index++) {
						startToEndChannel(start, end, d, lookup[lookupindex][index], 255);
					}
				}
				else if (_light.mode == MS4_App_Light_Mode_SCANNER_BW)
				{
					startToEndZigZag(start, end, 1, 255, 255, 255);
				}

				lookupindex = (lookupindex + 1) % 6;
				dir = (dir + 1) % 2;
			}
			else { // MS4_App_Light_Mode_SCANNER_RGB_BW
				leds.fillLEDs(0, 0, 0, brightness);
				leds.setLED(start, 255, 255, 255, brightness);
				leds.updateLEDs();
			}

			if (centerMode) {
				centerMode = 0;
			} else {
				centerMode = 1;
			}

		}

		return true;
	}

	void update() {
	};
	void reset() {
	};

	void startToEndChannel(uint8_t start, uint8_t end, int d, int channel, int color)
	{
		if (!hasContext()) return;

		auto& leds = context->getLEDs();
		int i;

		i = start;
		do {
			if (lightSubModeSelector()) {
				return;
			}

			leds.setAllChannel(channel, 0);
			leds.setChannel(i, channel, color);
			leds.updateLEDs();
			if (d)
				delay(d);
			if (i < end)
				i++;
			else
				i--;
		} while (i != end);
		leds.setAllChannel(channel, 0);
		leds.setChannel(i, channel, color);
		leds.updateLEDs();
	}

	void startToEndZigZag(uint8_t start, uint8_t end, int d, uint8_t r, uint8_t g, uint8_t b)
	{
		if (!hasContext()) return;

		auto& leds = context->getLEDs();
		uint8_t brightness = context->getBrightness();

		int i;
		uint8_t lastEnd = end;
		uint8_t currentStart = start;
		uint8_t currentEnd = start;

		if (end < start)
			end = start-1;
		else
			end = start+1;

		for (;;)
		{
			i = currentStart;
			do {
				if (lightSubModeSelector()) {
					return;
				}

				leds.fillLEDs(0, 0, 0, brightness);
				if (i >= 0 && i < MAX_LEDS)
				leds.setLED(i, r, g, b, brightness);
				leds.updateLEDs();
				if (d)
					delay(d);
				if (i < currentEnd)
					i++;
				else
					i--;
			} while (i != currentEnd);

			if (i == end)
			{
				if (end == lastEnd)
				{
					if (start < end)
						start++;
					else
						start--;
					if (start == end)
						break;
				}
				currentStart = end;
				currentEnd = start;
			}
			else
			{
				if (end != lastEnd)
				{
					if (end < lastEnd)
						end++;
					else
						end--;
				}
				currentStart = start;
				currentEnd = end;
			}
		}

		// last
		leds.fillLEDs(0, 0, 0, brightness);
		leds.setLED(i, r, g, b, brightness);
		leds.updateLEDs();
	}

};

#endif
