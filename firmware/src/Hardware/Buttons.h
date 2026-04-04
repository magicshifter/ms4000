/**
 * @file Buttons.h
 * @brief Button input handling with debouncing and event detection
 *
 * Provides single-click, long-click, and double-click detection
 * for buttons A, B, and Power using HAL GPIO abstraction.
 */

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "../HAL/hal_gpio.h"

class MagicShifterButtons {

  private:

  public:
  	bool enableABDoubleHit = false;

	// TODO: private state
	// state for button timing
	long msBtnAPressTime = 0;
	long msBtnPwrPressTime = 0;
	long msBtnBPressTime = 0;

	// state for double click timing
	long msBtnATTL = 0;
	long msBtnPwrTTL = 0;
	long msBtnBTTL = 0;

	bool msLongClickOK = true;

	// todo public properties? Logic for consuming buttons?
	// events for consumers true/false;
	bool msBtnAHit = false;
	bool msBtnPwrHit = false;
	bool msBtnBHit = false;

	bool msBtnALongHit = false;
	bool msBtnPwrLongHit = false;
	bool msBtnBLongHit = false;

	bool msBtnADoubleHit = false;
	bool msBtnPwrDoubleHit = false;
	bool msBtnBDoubleHit = false;

	long msBtnALastHitTime = 0;
	long msBtnBLastHitTime = 0;
	long msBtnPwrLastHitTime = 0;

	long msBtnALastLongHitTime = 0;
	long msBtnBLastLongHitTime = 0;
	long msBtnPwrLastLongHitTime = 0;

	long deltaMicros = 0;		// !J! todo: init? (msGlobals.ggCurrentMicros - msGlobals.ggLastMicros);

	//  button activity 
	bool msBtnActive = false;

	bool powerButtonPressed(void) {
		// had unusual high values on one of the 100x PCBs so this threshold is bigger now
		return msGlobals.ggLastADValue > BUTTON_PWR_THRESHOLD_DEFAULT;
	}; 


	/**
	 * @brief Initialize button GPIO pins
	 * Configures buttons A and B as inputs using HAL.
	 */
	void setup()
	{
		hal_gpio_set_mode(PIN_BUTTON_A, HAL_GPIO_MODE_INPUT);
		hal_gpio_set_mode(PIN_BUTTON_B, HAL_GPIO_MODE_INPUT);
	}


	void resetButtons() {
		msBtnActive = false;
		msBtnAHit = false;
		msBtnBHit = false;
		msBtnPwrHit = false;
		msBtnALongHit = false;
		msBtnBLongHit = false;
		msBtnPwrLongHit = false;
		msBtnADoubleHit = false;
		msBtnBDoubleHit = false;
		msBtnPwrDoubleHit = false;
	}


	/**
	 * @brief Process button state and detect events
	 * Called every frame to update button states and detect clicks.
	 * Implements debouncing and long/double-click detection.
	 */
	void step()
	{
		resetButtons();

		deltaMicros = (msGlobals.ggCurrentMicros - msGlobals.ggLastMicros);

		// Ensure button pins are configured as inputs
		hal_gpio_set_mode(PIN_BUTTON_A, HAL_GPIO_MODE_INPUT);
		hal_gpio_set_mode(PIN_BUTTON_B, HAL_GPIO_MODE_INPUT);

		// Read button A state (active low)
		hal_gpio_state_t btnA_state;
		hal_gpio_read(PIN_BUTTON_A, &btnA_state);

		if (btnA_state == HAL_GPIO_LOW)
		{
			msBtnActive = true;

			if (msBtnAPressTime)
				msBtnAPressTime += deltaMicros;
			else
				msBtnAPressTime = 1;
		} else {
			if (msLongClickOK && msBtnAPressTime >= MIN_TIME_LONG_CLICK) {
				msBtnALongHit = true;
				msBtnALastLongHitTime = millis();
			} else if (msBtnAPressTime >= MIN_TIME_CLICK) {
				if (enableABDoubleHit) {
					// long hits
					long _now = millis();
					if ((_now - msBtnALastHitTime) < MAX_TIME_DOUBLE_CLICK) {
						msBtnADoubleHit = true;
					} else {
						msBtnAHit = true;
					}
				}
				else
				{
					msBtnAHit = true;
				}
				msBtnALastHitTime = millis();
			}

			msBtnAPressTime = 0;
		}

		// Read button B state (active low)
		hal_gpio_state_t btnB_state;
		hal_gpio_read(PIN_BUTTON_B, &btnB_state);

		if (btnB_state == HAL_GPIO_LOW)
		{
			msBtnActive = true;

			if (msBtnBPressTime)
				msBtnBPressTime += deltaMicros;
			else
				msBtnBPressTime = 1;
		} else {
			if (msLongClickOK && msBtnBPressTime >= MIN_TIME_LONG_CLICK) {
				msBtnBLongHit = true;
				msBtnBLastLongHitTime = millis();
			} else if (msBtnBPressTime >= MIN_TIME_CLICK) {
				if (enableABDoubleHit) {
					// long hits
					long _now = millis();
					if ((_now - msBtnBLastHitTime) < MAX_TIME_DOUBLE_CLICK) {
						msBtnBDoubleHit = true;
					} else {
						msBtnBHit = true;
					}
				}
				else
				{
					msBtnBHit = true;
				}
				msBtnBLastHitTime = millis();
			}
			
			msBtnBPressTime = 0;
		}

		if (powerButtonPressed()) {

			msBtnActive = true;

			if (msBtnPwrPressTime)
				msBtnPwrPressTime += deltaMicros;
			else
				msBtnPwrPressTime = 1;
		} else {
			if (msBtnPwrPressTime >= MIN_TIME_LONG_CLICK) {
				msBtnPwrLongHit = true;
				msBtnPwrLastLongHitTime = millis();
			} else if (msBtnPwrPressTime >= MIN_TIME_CLICK) {
				// // long hits
				// long _now = millis();
				// if ((_now - msBtnPwrLastHitTime) < MAX_TIME_DOUBLE_CLICK) {
				// 	msBtnPwrDoubleHit = true;
				// } else {
					msBtnPwrHit = true;
				// }

				// msBtnPwrLastHitTime = millis();
			}

			msBtnPwrPressTime = 0;
		}
	}

	#define SYNC_CLICK_TIME_MS 250
	bool menuButtonsSelected() {
		if ((msBtnAHit && (millis() - msBtnBLastHitTime) < SYNC_CLICK_TIME_MS) ||
			(msBtnBHit && (millis() - msBtnALastHitTime) < SYNC_CLICK_TIME_MS) ||
			(msBtnALongHit && (millis() - msBtnBLastLongHitTime) < SYNC_CLICK_TIME_MS) ||
			(msBtnBLongHit && (millis() - msBtnALastLongHitTime) < SYNC_CLICK_TIME_MS)) {
			resetButtons();
			return true;
		}
		return false;
	}
};

#endif
