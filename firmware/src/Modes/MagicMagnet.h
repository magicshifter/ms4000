#ifndef _MAGNETOMETER_MODE_H
#define _MAGNETOMETER_MODE_H

//
// render the magnetometer sensor data
//

class MagicMagnetMode:public MagicShifterBaseMode {

  private:

	int magnet_submode =  msGlobals.pbuf.apps.magnet.mode;

#ifdef CONFIG_ENABLE_OSC
	// local OSC message
	OSCMessage lOSCMessage = OSCMessage("/magicshifter3000/magnetometer/LED/");
#endif

  public:

  	MagicMagnetMode() {
		 	modeName = "Compass";

	}

	void start() {

	}

	void stop() {
	}

	bool step() {
		if (!hasContext()) return true;

		// Note: Direct sensor register access and config still need msSystem
		extern MagicShifterSystem msSystem;
		extern MagicShifterGlobals msGlobals;

		static int autoCalResetCounter = 0;

		magnet_submode =  msGlobals.pbuf.apps.magnet.mode;

		auto& buttons = context->getButtons();
		auto& leds = context->getLEDs();
		auto& logger = context->getLogger();
		uint8_t brightness = context->getBrightness();

		if (buttons.isButtonAPressed()) {
			magnet_submode--;
			msSystem.msButtons.msBtnAHit = false;
		}
		if (buttons.isButtonBPressed()) {
			magnet_submode++;
			msSystem.msButtons.msBtnBHit = false;
		}

		if (magnet_submode < _MS4_App_Magnet_Mode_MIN)
			magnet_submode = _MS4_App_Magnet_Mode_MAX;

		if (magnet_submode > _MS4_App_Magnet_Mode_MAX)
			magnet_submode = _MS4_App_Magnet_Mode_MIN;

		msSystem.msSensor.readMagnetometerData(msGlobals.ggMagnet);

		if ((msSystem.msSensor.readRegister(0x5E) & 0x02)) {
			autoCalResetCounter++;
			if (autoCalResetCounter > 10) {
				// M_CTRL_REG2: Hybrid auto increment, Magnetic measurement min/max detection function reset
				msSystem.msSensor.writeRegister(0x5C, 0x24);
				autoCalResetCounter = 0;
			}
		} else {
			autoCalResetCounter = 0;
		}

		int degrees =
			int (atan2
				 (msGlobals.ggMagnet[YAXIS],
				  -msGlobals.ggMagnet[XAXIS]) * 180 / M_PI);

		logger.log((modeName + " degrees: " + String(degrees)).c_str());

		leds.fillLEDs(0, 0, 0, brightness);
		int lednr = map(abs(degrees), 0, 180, 0, 15);

		int degNorth = -degrees;
		int degSouth = 180 - degrees;

		if (degSouth>180) degSouth-=360;
		int ledNorth = map(abs(degNorth), 0, 180, 0, 15);
		int ledSouth = map(abs(degSouth), 0, 180, 0, 15);

		if (magnet_submode <= MS4_App_Magnet_Mode_BARS_DOT) {
			for (int lC = 0; lC < lednr; lC++)
				leds.setLED(lC, 0, 255, 0, brightness);

			for (int lC = lednr + 1; lC < MAX_LEDS; lC++)
				leds.setLED(lC, 255, 0, 0, brightness);
		}

		if ((magnet_submode == MS4_App_Magnet_Mode_BARS) ||
			(magnet_submode == MS4_App_Magnet_Mode_DOTS))
			leds.setLED(lednr, 0, 255, 0, brightness);

		if (magnet_submode == MS4_App_Magnet_Mode_BARS_DOT)
			leds.setLED(lednr, 0, 0, 255, brightness);

		if (magnet_submode == MS4_App_Magnet_Mode_OTHER) {
			leds.setLED(ledSouth, 0, 255, 0, brightness);
			leds.setLED(ledNorth, 255, 0, 0, brightness);
		}

 		msGlobals.pbuf.apps.magnet.mode = (MS4_App_Magnet_Mode)magnet_submode;

		leds.updateLEDs();

		// !J! hak: send OSC Msg.
#ifdef CONFIG_ENABLE_OSC
		lOSCMessage.add(lednr);
#endif

		delay(30);

		return true;

	}

};

#endif
