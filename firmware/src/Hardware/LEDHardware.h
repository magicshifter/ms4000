/**
 * @file LEDHardware.h
 * @brief LED Buffer functional interface to APA102/WS2801 LED controller
 *
 * Provides high-level LED control using HAL SPI and GPIO abstractions.
 */

#ifndef LEDHARDWARE_H
#define LEDHARDWARE_H

#include "../HAL/hal_spi.h"
#include "../HAL/hal_gpio.h"

/**
 * @brief SPI frequency selection based on LED type
 * APA102: 20 MHz, WS2801: 500 kHz
 */
#if (LED_TYPE == LED_TYPE_APA102)
#define SPI_FREQUENCY HAL_SPI_FREQ_APA102
#else
#define SPI_FREQUENCY HAL_SPI_FREQ_WS2801
#endif

/**
 * @brief LED control pin assignments
 * PIN_LED_ENABLE: GPIO pin to enable LED power
 * PIN_LED_DATA/CLOCK: Hardware SPI pins (fixed on ESP8266)
 */
#define PIN_LED_ENABLE 15
#define PIN_LED_DATA 13  // Hardware SPI MOSI (fixed)
#define PIN_LED_CLOCK 14 // Hardware SPI SCK (fixed)

/**
 * @brief Non-linear brightness correction lookup table
 *
 * Corrects for non-linear brightness perception in LEDs when using PWM.
 * Maps 4-bit input (0-15) to 8-bit PWM value with gamma correction.
 *
 * @see http://electronics.stackexchange.com/questions/1983/correcting-for-non-linear-brightness-in-leds-when-using-pwm/11100
 */
byte lookupBrightness[16] = {0x00, 0x02, 0x04, 0x07, 0x0B, 0x12, 0x1E, 0x28,
                             0x32, 0x41, 0x50, 0x64, 0x7D, 0xA0, 0xC8, 0xFF};

class MagicShifterLEDs {

  private:

	byte ledBuffer[RGB_BUFFER_SIZE + 8];
	byte clearBuffer[RGB_BUFFER_SIZE + 8];

	byte *RGB_COLORS = ledBuffer + 4;


  public:
	void saveBuffer(byte * buffer) {
		for (int i = 0; i < RGB_BUFFER_SIZE; i++) {
			buffer[i] = RGB_COLORS[i];
	}} void loadBuffer(byte * buffer) {
		for (int i = 0; i < RGB_BUFFER_SIZE; i++) {
			RGB_COLORS[i] = buffer[i];

			// i+=4..
			// RGB_COLORS[i] = buffer[i+3];
			// RGB_COLORS[i+1] = buffer[i+0];
			// RGB_COLORS[i+2] = buffer[i+1];
			// RGB_COLORS[i+3] = buffer[i+2];

		}
	}

	void loadBufferShort(byte * buffer) {
		for (int i = 0; i < RGB_BUFFER_SIZE; i++) {
			if ((i & 4) == 0)
				RGB_COLORS[i] = buffer[i];
			else {
				RGB_COLORS[i] = lookupBrightness[(buffer[i]) & 0xF] >> 3;
			}

			// i+=4..
			// RGB_COLORS[i] = buffer[i+3];
			// RGB_COLORS[i+1] = buffer[i+0];
			// RGB_COLORS[i+2] = buffer[i+1];
			// RGB_COLORS[i+3] = buffer[i+2];

		}
	}

	void loadBufferLong(byte * buffer) {
		for (int i = 0; i < RGB_BUFFER_SIZE; i++) {
			if ((i & 4) == 0)
				RGB_COLORS[i] = buffer[i];
			else {
				RGB_COLORS[i] = lookupBrightness[buffer[i] >> 4] >> 3;
			}

			// i+=4..
			// RGB_COLORS[i] = buffer[i+3];
			// RGB_COLORS[i+1] = buffer[i+0];
			// RGB_COLORS[i+2] = buffer[i+1];
			// RGB_COLORS[i+3] = buffer[i+2];

		}
	}

	void disableLEDHardware() {
		pinMode(PIN_LED_ENABLE, INPUT);
	}


	/**
	 * @brief Initialize LED hardware (GPIO and SPI)
	 * Sets up LED enable pin and initializes SPI bus using HAL.
	 */
	void initLEDHardware()
	{
		// Enable LED power supply
		hal_gpio_set_mode(PIN_LED_ENABLE, HAL_GPIO_MODE_OUTPUT);
		hal_gpio_write(PIN_LED_ENABLE, HAL_GPIO_HIGH);

		// Initialize SPI bus for LED communication
		hal_spi_init(SPI_FREQUENCY);
	}

	void bootSwipe() {

		// swipe colors
		for (byte idx = 0; idx < MAX_LEDS; idx++) {
			int i2 = idx % 7 + 1;

			setLED(idx, (i2 & 1) ? 255 : 0, (i2 & 2) ? 255 : 0, (i2 & 4) ? 255 : 0, msGlobals.ggBrightness);
			updateLEDs();
			delay(30);
		}
		for (byte idx = 0; idx < MAX_LEDS; idx++) {
			setLED(idx, 0, 0, 0);
			updateLEDs();
			delay(30);
		}
	}

	void errorSwipe() {

		// swipe colors
		for (byte idx = 0; idx < MAX_LEDS; idx++) {
			setLED(idx, (idx & 1) ? 255 : 0, 0, 0, msGlobals.ggBrightness);
			updateLEDs();
			delay(30);
		}
		for (byte idx = 0; idx < MAX_LEDS; idx++) {
			setLED(idx, 0, 0, 0, 1);
			updateLEDs();
			delay(30);
		}
	}


	void initLEDBuffer() {
		ledBuffer[0] = 0;
		ledBuffer[1] = 0;
		ledBuffer[2] = 0;
		ledBuffer[3] = 0;


		ledBuffer[RGB_BUFFER_SIZE + 0] = 0;
		ledBuffer[RGB_BUFFER_SIZE + 1] = 0;
		ledBuffer[RGB_BUFFER_SIZE + 2] = 0;
		ledBuffer[RGB_BUFFER_SIZE + 4] = 0;



		for (int i = 0; i < RGB_BUFFER_SIZE + 8; i += 4) {
			clearBuffer[i] = (i < 4
							  || i >= RGB_BUFFER_SIZE + 4) ? 0 : 0xFF;
			clearBuffer[i + 1] = 0;
			clearBuffer[i + 2] = 0;
			clearBuffer[i + 3] = 0;
		}

		fillLEDs(0, 0, 0, 0);
	}

	/**
	 * @brief Set a single color channel for an LED
	 *
	 * @param index LED index (0 to MAX_LEDS-1)
	 * @param channel Channel number (0=B, 1=G, 2=R, 3=brightness for APA102)
	 * @param value Channel value (0-255)
	 */
	void setLEDChannel(int index, int channel, int value)
	{
#if (LED_TYPE == LED_TYPE_APA102)
		int idx = index << 2;
		if (channel == 3)
		{
			// Set brightness byte: [111BBBBB] where BBBBB is 5-bit brightness
			RGB_COLORS[idx] = APA102_BRIGHTNESS_MASK | (value & APA102_BRIGHTNESS_MAX);
		}
		else
		{
			RGB_COLORS[idx + 1 + channel] = value;
		}
#else
		int idx = index * 3;
		RGB_COLORS[idx + channel] = value;
#endif
	}

	// void setLED(int index, byte r, byte g, byte b)
	// {
	// 	setLED(index,r,g,b,0x40); // !J! debug
	// }

	/**
	 * @brief Set LED color and brightness
	 *
	 * @param index LED index (0 to MAX_LEDS-1)
	 * @param r Red value (0-255)
	 * @param g Green value (0-255)
	 * @param b Blue value (0-255)
	 * @param brightness Brightness (0-31 for APA102, ignored for WS2801)
	 */
	void setLED(int index, byte r, byte g, byte b, byte brightness = APA102_BRIGHTNESS_MAX)
	{
#if (LED_TYPE == LED_TYPE_APA102)
		int idx = index << 2;
		// APA102 brightness byte: [111BBBBB] where BBBBB is 5-bit brightness
		RGB_COLORS[idx] = APA102_BRIGHTNESS_MASK | (brightness & APA102_BRIGHTNESS_MAX);
		RGB_COLORS[idx + 1] = b;
		RGB_COLORS[idx + 2] = g;
		RGB_COLORS[idx + 3] = r;
#else
		int idx = index * 3;
		RGB_COLORS[idx + 0] = b;
		RGB_COLORS[idx + 1] = g;
		RGB_COLORS[idx + 2] = r;
#endif
	}

	void fillLEDs(byte r, byte g, byte b, byte brightness = 0x1F) {
		for (int idx = 0; idx < MAX_LEDS; idx++) {
			setLED(idx, r, g, b, brightness);
		}
	}

	/**
	 * @brief Update LED hardware with buffered data
	 * Sends LED buffer to hardware via SPI using HAL.
	 */
	void updateLEDs()
	{
#if (LED_TYPE == LED_TYPE_APA102)
		hal_spi_write(ledBuffer, RGB_BUFFER_SIZE + 8);
#else
		hal_spi_write(RGB_COLORS, RGB_BUFFER_SIZE);
#endif
	}

	/**
	 * @brief Fast clear all LEDs
	 * Writes pre-initialized clear buffer to turn off all LEDs quickly.
	 */
	void fastClear()
	{
		hal_spi_write(clearBuffer, RGB_BUFFER_SIZE + 8);
	}


	void setAllChannel(uint8_t channel, uint8_t value)
	{
		for (int i = 0; i < MAX_LEDS; i++) {
			setChannel(i, channel, value);
		}
	}

	void setChannel(uint8_t index, uint8_t channelIndex, uint8_t value)
	{
	  if (index >= 0 && index < MAX_LEDS && channelIndex >= 0 && channelIndex < 3)
	  {
#if (LED_TYPE == LED_TYPE_APA102)
			int idx = index << 2;
			RGB_COLORS[idx + 1 + channelIndex] = value;
#else
			int idx = index * 3;
			RGB_COLORS[idx + channelIndex] = value;
#endif
	  }
	}

};


#endif
// LEDHARDWARE_H
