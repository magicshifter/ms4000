//
// Accelerometer modules - written for MMA8452/FXOS8700CQ
//
#include "../HAL/hal_i2c.h"
#include <math.h>

/**
 * @brief Legacy register name aliases for backward compatibility
 * These map to the standardized FXOS_REG_* names defined in hal_i2c.h
 */
#define OUT_X_MSB FXOS_REG_OUT_X_MSB
#define MAG_REG FXOS_REG_MAG_STATUS
#define XYZ_DATA_CFG FXOS_REG_XYZ_DATA_CFG
#define WHO_AM_I FXOS_REG_WHO_AM_I
#define CTRL_REG1 FXOS_REG_CTRL_REG1
#define MAGNETOMETER_REGISTER FXOS_REG_MAG_DATA_START

/**
 * @brief Accelerometer full-scale range setting
 * Sets full-scale range to +/-2, 4, or 8g. Used to calculate real g values.
 */
#define GSCALE 8

/**
 * @brief I2C pin assignments (moved to BoardConfig in future)
 * PIN_I2C_DATA: GPIO5 (D1 on NodeMCU) - blue wire
 * PIN_I2C_CLOCK: GPIO4 (D2 on NodeMCU) - purple wire
 */
#define PIN_I2C_DATA 5
#define PIN_I2C_CLOCK 4

/**
 * @brief Accelerometer axis indices (confirmed via hardware testing)
 * Coordinate system: Right-handed, X=forward, Y=right, Z=down
 */
#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2


#ifndef CONFIG_ENABLE_ACCEL
void setupSensor()
{
};

void readAccelData(int *destination)
{
};
#endif


// TODO : Flesh this out?
class MagicShifterAccelerometer {

  public:

	void step() {
		// outside time-frame
#ifdef CONFIG_ENABLE_ACCEL
		readAccelData(msGlobals.ggAccelCount);

		for (int i = 0; i < 3; i++) {
			msGlobals.ggAccel[i] = (float) msGlobals.ggAccelCount[i] / ((1 << 12) / (2 * GSCALE));	// get actual g value, this depends on scale being set
		}
#endif
	}

	/**
	 * @brief Initialize I2C bus for sensor communication
	 * Uses HAL layer for standardized I2C initialization.
	 * Clock rate: 500kHz (max safe for ESP8266)
	 */
	void initI2C()
	{
		hal_i2c_init(PIN_I2C_DATA, PIN_I2C_CLOCK, 500000);
	}


#ifdef CONFIG_ENABLE_ACCEL
	/**
	 * @brief Read multiple bytes sequentially from sensor registers
	 * Uses HAL I2C layer with error handling.
	 *
	 * @param addressToRead Starting register address
	 * @param bytesToRead Number of bytes to read
	 * @param dest Destination buffer
	 */
	void readRegisters(byte addressToRead, int bytesToRead, byte *dest)
	{
		hal_status_t status = hal_i2c_read_registers(MMA8452_ADDRESS,
		                                             addressToRead,
		                                             dest,
		                                             bytesToRead);

		if (status != HAL_OK)
		{
			Serial.println("Sensor Package unavailable!!");
			msGlobals.ggFault = FAULT_NO_ACCELEROMETER;
			delay(10);
		}
	}

	/**
	 * @brief Read a single byte from sensor register
	 * Uses HAL I2C layer with error handling.
	 *
	 * @param addressToRead Register address
	 * @return Byte value read from register
	 */
	byte readRegister(byte addressToRead)
	{
		byte data = 0;
		hal_status_t status = hal_i2c_read_register(MMA8452_ADDRESS, addressToRead, &data);

		if (status != HAL_OK)
		{
			Serial.println("Sensor Package unavailable!!");
			msGlobals.ggFault = FAULT_NO_ACCELEROMETER;
			delay(10);
		}

		return data;
	}

	/**
	 * @brief Write a single byte to sensor register
	 * Uses HAL I2C layer.
	 *
	 * @param addressToWrite Register address
	 * @param dataToWrite Byte value to write
	 */
	void writeRegister(byte addressToWrite, byte dataToWrite)
	{
		hal_i2c_write_register(MMA8452_ADDRESS, addressToWrite, dataToWrite);
	}


	/**
	 * @brief Set sensor to standby mode
	 * Must be in standby to change most register settings.
	 */
	void MMA8452Standby()
	{
		byte c = readRegister(CTRL_REG1);
		// Clear the active bit to go into standby
		writeRegister(CTRL_REG1, c & ~FXOS_CTRL_REG1_ACTIVE);
	}

	/**
	 * @brief Set sensor to active mode
	 * Sensor must be in active mode to output data.
	 */
	void MMA8452Active()
	{
		byte c = readRegister(CTRL_REG1);
		// Set the active bit to begin detection
		writeRegister(CTRL_REG1, c | FXOS_CTRL_REG1_ACTIVE);
	}

	void readMagnetometerData(int *destination) {
		byte rawData[12];		// x/y/z accel register data stored here
		readRegisters(MAGNETOMETER_REGISTER, 6, rawData);	// Read the six raw data 

		for (uint8_t i = 0; i < 3; i++) {
			short val =
				((short) rawData[2 * i + 0] << 8) | (short) rawData[2 * i +
																	1];
			destination[i] = val;
		}

	}

	void readAccelData(int *destination) {
		long _ti = micros();

		byte rawData[6];		// x/y/z accel register data stored here

		readRegisters(OUT_X_MSB, 6, rawData);	// Read the six raw data registers into data array


		// Loop to calculate 12-bit ADC and g value for each axis
		for (int i = 0; i < 3; i++) {
			int gCount = (rawData[i * 2] << 8) | rawData[(i * 2) + 1];	//Combine the two 8 bit registers into one 12-bit number
			gCount >>= 4;		//The registers are left align, here we right align the 12-bit integer

			// If the number is negative, we have to make it so manually (no 12-bit data type)
			if (rawData[i * 2] > 0x7F) {
				gCount -= 0x1000;
			}

			destination[i] = gCount;	//Record this gCount into the 3 int array
		}

		msGlobals.ggAccelTime = micros() - _ti;
	}



	// Initialize the MMA8452 registers
	// See the many application notes for more info on setting all of these registers:
	// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
	bool setupSensor() {
		bool success = false;

		do {
			// Serial.print("testin accel: ");
			// Serial.println(MMA8452_ADDRESS, HEX);

			byte c = readRegister(WHO_AM_I);	// Read WHO_AM_I register
			// Serial.print("read: ");
			// Serial.println(c, HEX);

			if (c == MMA8452_ID)	// WHO_AM_I should always be 0x2A
			{
				// Serial.println("MMA8452Q is online...");
				success = true;
			} else {
				// Serial.println("Could not connect to MMA8452Q: expected 0x");
				// Serial.println(String(MMA8452_ID, HEX));
				// Serial.println(" but received 0x");
				// Serial.println(String(c, HEX));
				delay(10);
			}
			/* code */
		} while (0);			//while(!success);

		// Magnetometer configuration
		bool autoCalibrateMagnetometer = true;
		if (autoCalibrateMagnetometer)
		{
			// Soft reset the sensor
			writeRegister(FXOS_REG_CTRL_REG2, FXOS_CTRL_REG2_RST);
			delay(1); // Wait for reset to complete

			// Set magnetometer vector magnitude threshold
			// 1000 counts = 100.0uT
			int magThreshold = 1000;
			writeRegister(FXOS_REG_M_VECM_THS_LSB, ((0x80 | magThreshold) & 0xFF));
			writeRegister(FXOS_REG_M_VECM_THS_MSB, magThreshold >> 8);

			// Set magnetometer sample count: 1 * 20ms = 20ms
			// Note: Sample rate doubles in hybrid mode
			writeRegister(FXOS_REG_M_VECM_CNT, 0x01);

			// Configure magnetometer vector magnitude detection:
			// - Event latching enabled (m_vecm_ele=1)
			// - Use initial reference values (m_vecm_initm=1)
			// - Do not update reference (m_vecm_updm=1)
			// - Enable detection feature (m_vecm_en=1)
			writeRegister(FXOS_REG_M_VECM_CFG, FXOS_M_VECM_CFG_INIT_MODE);
		}

		MMA8452Standby();		// Must be in standby to change registers
		// Set up the full scale range to 2, 4, or 8g.
		byte fsr = GSCALE;
		if (fsr > 8)
			fsr = 8;			//Easy error check
		fsr >>= 2;				// Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
		writeRegister(XYZ_DATA_CFG, fsr);
		//The default data rate is 800Hz and we don't modify it in this example code

		// Configure magnetometer control registers
		// M_CTRL_REG1: Hybrid mode with oversampling ratio = 32
		// Auto-calibration enabled if autoCalibrateMagnetometer is true
		writeRegister(FXOS_REG_M_CTRL_REG1,
		              (autoCalibrateMagnetometer ? FXOS_M_CTRL_REG1_ACAL : 0x00) |
		                  FXOS_M_CTRL_REG1_HYBRID);
		// M_CTRL_REG2: Enable hybrid auto-increment mode
		writeRegister(FXOS_REG_M_CTRL_REG2, FXOS_M_CTRL_REG2_HYBRID);

		MMA8452Active();		// Set to active to start reading

		return success;
	}
#endif
};
