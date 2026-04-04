//
// the main mode selector for the ms3000 interface
//

class ModeSelectorMode:public MagicShifterBaseMode {

  private:
	// MagicShifterImageAbstr *msImage;

	// local POVMode for the text
	MagicPOVMode lPOVMode;
	MagicShifterImageText msMagicShakeText;
	
	uint32_t _currentMode = 0;

  public:

	void setText(const char *label) {
		// Note: Still uses msGlobals for font/bitmap data (not hardware-dependent)
		extern MagicShifterGlobals msGlobals;

		// MSColor aRED = { 0xff, 0x00, 0x00 };
		MSColor aWhite = { 0xFF, 0xFF, 0xFF };
		Coordinate_s tPos;

		msMagicShakeText.resetTexts();

		tPos.x = 0;
		tPos.y = 0;
		msGlobals.ggtBitmap6x8.color = aWhite;
		msMagicShakeText.plotTextString((char *) label,
										msGlobals.ggtBitmap6x8, tPos);
		lPOVMode.setImage(&msMagicShakeText);
	}

	virtual void start() {
		// Note: Still uses msGlobals for mode list and UI state (not hardware-dependent)
		extern MagicShifterGlobals msGlobals;

		_currentMode = msGlobals.ui.currentMode;

		setCurrentMode(_currentMode);

	}

	// stop the mode
	virtual void stop(void) {
		lPOVMode.setImage(NULL);
	}

	void setCurrentMode(uint32_t idx) {
		extern MagicShifterGlobals msGlobals;

		if (idx > msGlobals.ggModeList.size())
				idx = msGlobals.ggModeList.size() - 1;

		if (idx < 0) {
			idx = msGlobals.ggModeList.size() - 1;
		}

		if (idx >= msGlobals.ggModeList.size()) {
			idx = 0;
		}

		_currentMode = idx;

		setText(msGlobals.ggModeList[_currentMode]->modeName.c_str());
	}

// step through a frame of the mode
	int select() {
		if (!hasContext()) return -1;

		uint _index = _currentMode;
		uint _posIdx = _index;

		auto& buttons = context->getButtons();
		auto& leds = context->getLEDs();
		uint8_t brightness = context->getBrightness();

		// button handling
		if (buttons.isPowerButtonPressed()) {
			// blink selected mode 4 times
			for (byte i = 0; i < 4; i++) {
				leds.setLED(_posIdx, 255, 255, 255, brightness);
				leds.updateLEDs();
				delay(50);
				leds.setLED(_posIdx, 0, 0, 0);
				leds.updateLEDs();
				delay(50);
			}
			return _index;
		}
		// cycle through the texts ..
		if (buttons.isButtonAPressed()) {
			// Note: Direct button flag clearing still needed until button API is enhanced
			extern MagicShifterSystem msSystem;
			msSystem.msButtons.msBtnAHit = false;	// !J! todo: button callbacks
			_currentMode--;
			setCurrentMode(_currentMode);
		}
		// cycle through the texts ..
		if (buttons.isButtonBPressed()) {
			extern MagicShifterSystem msSystem;
			msSystem.msButtons.msBtnBHit = false;	// !J! todo: button callbacks
			_currentMode++;
			setCurrentMode(_currentMode);
		}

		if (lPOVMode.step()) {
			return -1;
		} else {
			int selectedMode = 1 + (_index % 7);

			int r = (selectedMode >> 0) % 2 ? 255 : 0;
			int g = (selectedMode >> 1) % 2 ? 255 : 0;
			int b = (selectedMode >> 2) % 2 ? 255 : 0;

			for (byte j = 0; j < 16; j++) {
				if (j == _posIdx) {
					leds.setLED(j, r, g, b, brightness);
				} else {
					leds.setLED(j, 0, 0, 0, brightness);
				}
			}

			leds.updateLEDs();
		}

		return -1;
	}
};
