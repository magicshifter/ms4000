class MagicRemoteMode : public MagicShifterBaseMode {

  public:
  	MagicRemoteMode() {
  		modeName = "Remote";
  	}

	virtual void start() {
		if (!hasContext()) return;

		// Note: Direct access to ggRGBLEDBuf still needed (shared buffer)
		extern MagicShifterGlobals msGlobals;
		uint8_t brightness = context->getBrightness();

		for(int i=0;i<RGB_BUFFER_SIZE;i+=4) {
			msGlobals.ggRGBLEDBuf[i] = brightness | 0xe0;
			msGlobals.ggRGBLEDBuf[i+1] = i * 4;
			msGlobals.ggRGBLEDBuf[i+2] = 255 - i * 16;
			msGlobals.ggRGBLEDBuf[i+3] = 0;
		}
	}
	virtual void stop(void) {
	}
	virtual bool step(void) {
		if (!hasContext()) return true;

		extern MagicShifterGlobals msGlobals;
		auto& leds = context->getLEDs();

		leds.loadBuffer(msGlobals.ggRGBLEDBuf);
		leds.updateLEDs();
		delay(10);
		return true;
	}
};