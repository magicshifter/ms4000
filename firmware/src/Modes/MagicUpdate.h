//
// OTA Updater Mode
//


class MagicUpdateMode : public MagicShifterBaseMode {

public:

	MagicUpdateMode() {
		modeName = "Update";
	}

	virtual void start() {
		if (!hasContext()) return;

		extern MagicShifterSystem msSystem;
		extern MagicShifterGlobals msGlobals;

		auto& leds = context->getLEDs();
		auto& logger = context->getLogger();
		uint8_t brightness = context->getBrightness();

		// set a basic background to start with ..
		for(int i=0;i<RGB_BUFFER_SIZE;i+=4) {
			msGlobals.ggRGBLEDBuf[i] = brightness | 0xe0;
			msGlobals.ggRGBLEDBuf[i+1] = i * 2;
			msGlobals.ggRGBLEDBuf[i+2] = 255 - i * 2;
			msGlobals.ggRGBLEDBuf[i+3] = 128;
		}

		leds.loadBuffer(msGlobals.ggRGBLEDBuf);
		leds.updateLEDs();


		ArduinoOTA.onStart([this]() {
			// set LED's to start-state:
			if (!hasContext()) return;
			context->getLogger().log(" update: start ");
			context->getLEDs().fillLEDs(0, 64, 0, context->getBrightness());
			context->getLEDs().updateLEDs();
		});

		ArduinoOTA.onProgress([this](int total, int size) {
			if (!hasContext()) return;
			//int percent = total / size;

			context->getLEDs().fillLEDs(0, 64, 0, context->getBrightness());
			context->getLEDs().updateLEDs();
		});

		ArduinoOTA.onEnd([this]() {
			// set LED's to end-state:
			if (!hasContext()) return;
			context->getLogger().log(" update: end");
			context->getLEDs().fillLEDs(128, 32, 64, context->getBrightness());
			context->getLEDs().updateLEDs();
		});

		ArduinoOTA.onError([this](ota_error_t error) {
			if (!hasContext()) return;
			context->getLogger().log(" update: error");
			context->getLEDs().fillLEDs(255, 0, 0, context->getBrightness());
			context->getLEDs().updateLEDs();
		});


		ArduinoOTA.begin();

		logger.log(" update: start (mode) ");

	}

	virtual void stop(void) {
		if (!hasContext()) return;

		extern MagicShifterGlobals msGlobals;

		context->getLogger().log(" update: stop (mode)");

		uint8_t brightness = context->getBrightness();

		// set a basic background to start with ..
		for(int i=0;i<RGB_BUFFER_SIZE;i+=4) {
			msGlobals.ggRGBLEDBuf[i] = brightness | 0xe0;
			msGlobals.ggRGBLEDBuf[i+1] = i * 2;
			msGlobals.ggRGBLEDBuf[i+2] = 255 - i * 2;
			msGlobals.ggRGBLEDBuf[i+3] = 128;
		}
	}

	virtual bool step(void) {
		if (!hasContext()) return true;

		extern MagicShifterGlobals msGlobals;

		ArduinoOTA.handle();

		auto& leds = context->getLEDs();
		leds.loadBuffer(msGlobals.ggRGBLEDBuf);
		leds.updateLEDs();

		delay(10);

		return true;
	}

};
