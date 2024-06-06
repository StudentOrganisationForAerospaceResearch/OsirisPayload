/*
 * LSM6DSO.hpp
 *
 *  Created on: Jun 1, 2024
 *      Author: sean
 */

#include <cstdint>
#include "main_system.hpp"

#ifndef SENSORS_INC_LSM6DSO_HPP_
#define SENSORS_INC_LSM6DSO_HPP_

#define INT1_CTRL 0x0D
#define WHO_AM_I 0x0F
#define CRTL_A 0x10 // accelerometer
#define CRTL_G 0x11 // gyro

#define CTRL_6 0x15
#define CTRL_7 0x16

#define OUTXG_L 0x22
#define OUTXG_H 0x23
#define OUTYG_L 0x24
#define OUTYG_H 0x25
#define OUTZG_L 0x26
#define OUTZG_H 0x27

#define OUTXA_L 0x28
#define OUTXA_H 0x29
#define OUTYA_L 0x2A
#define OUTYA_H 0x2B
#define OUTZA_L 0x2C
#define OUTZA_H 0x2D

#define X_OFS 0x73
#define Y_OFS 0x74
#define Z_OFS 0x75

enum MAX_G {
	G4 = 0,
	G32 = 1,
	G8 = 2,
	G16 = 3
};
enum MAX_DPS {
	DPS125  = 0b0001,
	DPS250  = 0b0000,
	DPS500  = 0b0010,
	DPS1000 = 0b0100,
	DPS2000 = 0b0110
};
enum WEIGHT {
	STRONG = 1,
	WEAK = 0
};


class LSM6DSO {
private:
	I2C_HandleTypeDef i2c;
//	SPI_HandleTypeDef spi;
//	bool usingSPI;

	float AccelMaxArray[4] = {4*9.81, 32*9.81, 8*9.81, 16*9.81}; // m/s^2
	unsigned short currentAccelMax = 0; //ptr into above array

	float GyroMaxArray[4] = {250, 500, 1000, 2000}; // m/s^2
	unsigned short currentGyroMax = 0; // degrees per second

	const static uint8_t DEFAULT_SPEED = 0b1010;


	bool initStatus = false;
	bool usingInterrupts = false;
	bool readReg(uint16_t regAddr, uint8_t *output);
	bool writeReg(uint16_t regAddr, uint8_t value);
	float convertToMS2(const int16_t value) const;
	float convertToDPS(const int16_t value) const;
	//bool disableBuffer(); Disabled by default so I don't care and won't be writing this
public:
	const static uint8_t CORRECT_ID = 0x6C;
	const static uint8_t I2C_READ_ADDRESS = 0xD5;
	const static uint8_t I2C_WRITE_ADDRESS = 0xD4;
	LSM6DSO(I2C_HandleTypeDef &i2c);
//	LSM6DSO(SPI_HandleTypeDef &spi);

	// Returns True on Success
	bool init(bool useInterrupts = false, uint16_t interruptPin = -1);
	bool setInterrupts(bool useInterrupts);
	// Set initial data
	/*
	 * @brief changes the speed of the polling of the Accelerometer
	 */
	bool setAccelSpeed(uint8_t speed);
	/*
	 * @brief changes the speed of the polling of the Accelerometer
	 */
	bool setGyroSpeed(uint8_t speed);
	// see above ENUMS they should just work
	// **	Changing one of these does ruin the offset!	**
	bool setAccelMax(MAX_G max);
	bool setGyroMax(MAX_DPS max);

	bool setAccelOffsetState(bool on);
	// The data sheet definitely does indicated how to calculate the value of these such that an offset
	// could be given in m/s^2 as there was like things on the milliGefore per lowest significant bit
	// but I couldn't figure it out so TODO figure that out
	bool setAccelOffset(const int8_t x, const int8_t y, const int8_t z, WEIGHT weight);
	// bool setGyroOffset(float &pitch, float &roll, float &yaw); this doesn't exist lol

	// Actually read the data without using interrupts
	bool readLinearAccel(float &xOut, float &yOut, float &zOut);
	bool readAngularAccel(float &pitch,float &roll ,float &yaw);
};


#endif /* SENSORS_INC_LSM6DSO_HPP_ */
