/*
 * LSM6DSO.cpp
 *
 *  Created on: May 28, 2024
 *      Author: seann
 */

#include "Inc//LSM6DSO.hpp"

LSM6DSO::LSM6DSO(I2C_HandleTypeDef &i2c) {
	this->i2c = i2c;
}

bool LSM6DSO::init(bool useInterrupts, uint16_t interruptPin)
{
	// pretty sure this should never happen
	if (interruptPin == -1 && usingInterrupts) return false;

	uint8_t buf = 0;
	initStatus = readReg(WHO_AM_I, &buf);
	initStatus = initStatus && setAccelSpeed(DEFAULT_SPEED) &&
			setGyroSpeed(DEFAULT_SPEED) && (buf == CORRECT_ID);
	// if we successfully set it, set it
	if (setInterrupts(useInterrupts) && initStatus) usingInterrupts = useInterrupts;
	return initStatus;
}
bool LSM6DSO::setInterrupts(bool useInterrupts)
{
	// make sure we are initialized
	if (!initStatus) return false;
	// don't change anything if we don't have to change anything!
	if (useInterrupts == usingInterrupts) return true;
	uint8_t currentState = 0;
	if (!readReg(INT1_CTRL, &currentState)) return false;

	if (usingInterrupts)
		currentState = currentState | (0b00000011);
	else
		currentState = (currentState & 0b11111100);
	return writeReg(INT1_CTRL, currentState);
}

bool LSM6DSO::setAccelSpeed(uint8_t speed)
{
	if (speed > 0b1010 || !initStatus) return false;
	uint8_t currentState = 0;
	if (!readReg(CRTL_A, &currentState)) return false;

	currentState = (currentState & 0b00001111) | (speed << 4);

	return writeReg(CRTL_A, currentState);
}

bool LSM6DSO::setGyroSpeed(uint8_t speed)
{
	if (speed > 0b1010 || !initStatus) return false;
	uint8_t currentState = 0;
	if (!readReg(CRTL_G, &currentState)) return false;

	currentState = (currentState & 0b00001111) | (speed << 4);

	return writeReg(CRTL_G, currentState);
}

bool LSM6DSO::readReg(uint16_t regAddr, uint8_t *output)
{
	uint8_t buf = 0xAA;
	if (HAL_OK != HAL_I2C_Mem_Read(&i2c, I2C_READ_ADDRESS, regAddr, 1, &buf, 1, 100))
	{
		return false;
	}
	*output = buf;
	//HAL_I2C_Master_Transmit(&i2c, I2C_WRITE_ADDRESS, )
	//HAL_I2C_Master_Receive(&i2c, I2C_READ_ADDRESS, &buf, 1, 100);
	return true;
}
bool LSM6DSO::setAccelMax(MAX_G max)
{
	if (initStatus == false) return false;

	uint8_t currentState = 0;
	if (!readReg(CRTL_A, &currentState)) return false;

	currentState = (currentState & 0b11110000) | (max<<2);

	if (writeReg(CRTL_A, currentState))
	{
		currentAccelMax = max;
		return true;
	}
	else return false;

}
bool LSM6DSO::setGyroMax(MAX_DPS max)
{
	if (initStatus == false) return false;
	uint8_t currentState = 0;
	if (!readReg(CRTL_G, &currentState)) return false;

	currentState = (currentState & 0b11110000) | (max<<1);
	if (writeReg(CRTL_G, currentState))
		{
			currentGyroMax = max;
			return true;
		}
	else return false;
}
bool LSM6DSO::setAccelOffsetState(bool on)
{
	uint8_t currentState = 0;
	if (!readReg(CTRL_7, &currentState)) return false;
	currentState = (currentState & 0b11110010) | (on << 1);
	return writeReg(CTRL_7, currentState);
}

bool LSM6DSO::setAccelOffset(const int8_t x, const int8_t y, const int8_t z, WEIGHT weight)
{
	if (initStatus == false) return false;
	uint8_t currentState = 0;
	if (!readReg(CTRL_6, &currentState)) return false;
	currentState = (currentState & 0b11110111) | (weight << 3);
	if (!writeReg(CTRL_6, currentState)) return false;
	return writeReg(X_OFS,x) && writeReg(Y_OFS,y) && writeReg(Z_OFS,z);
}

bool LSM6DSO::writeReg(uint16_t regAddr, uint8_t value)
{
	return (HAL_OK == HAL_I2C_Mem_Write(&i2c, I2C_WRITE_ADDRESS, regAddr, 1, &value, 1, 100));
}

// Should be made static but I'm lazy
float LSM6DSO::convertToMS2(const int16_t value) const
{
	return (((float) value) / 32767.0f) * AccelMaxArray[currentAccelMax];
}
float LSM6DSO::convertToDPS(const int16_t value) const
{
	if (currentGyroMax & 0b1) return (((float) value) / 32767.0f) * 125;
	return (((float) value) / 32767.0f / 9.81f) * GyroMaxArray[currentGyroMax >> 1];
}

bool LSM6DSO::readLinearAccel(float &xOut, float &yOut, float &zOut)
{
	if (initStatus == false) return false;

	int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;
	uint8_t value;
	for (uint16_t i = OUTZA_H; i >= OUTXA_L; i--)
	{
		if (readReg(i, &value))
		{
			switch (i) {
			case OUTZA_H:
				z = z | ((uint16_t) value) << 8;
				break;
			case OUTZA_L:
				z = z | value;
				break;
			case OUTYA_H:
				y = y | ((uint16_t) value) << 8;
				break;
			case OUTYA_L:
				y = y | value;
				break;
			case OUTXA_H:
				x = x | ((uint16_t) value) << 8;
				break;
			case OUTXA_L:
				x = x | value;
				break;
			default:
				return false;
			}
		}
	}
	xOut = convertToMS2(x);
	yOut = convertToMS2(y);
	zOut = convertToMS2(z);
	return true;


}
bool LSM6DSO::readAngularAccel(float &pitch,float &roll ,float &yaw)
{
	if (initStatus == false) return false;

	int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;
	uint8_t value;
	for (uint16_t i = OUTZG_H; i >= OUTXG_L; i--)
	{
		if (readReg(i, &value))
		{
			switch (i) {
			case OUTZG_H:
				z = z | ((uint16_t) value) << 8;
				break;
			case OUTZG_L:
				z = z | value;
				break;
			case OUTYG_H:
				y = y | ((uint16_t) value) << 8;
				break;
			case OUTYG_L:
				y = y | value;
				break;
			case OUTXG_H:
				x = x | ((uint16_t) value) << 8;
				break;
			case OUTXG_L:
				x = x | value;
				break;
			default:
				return false;
			}
		}
	}
	pitch = convertToDPS(x);
	roll = convertToDPS(y);
	yaw = convertToDPS(z);
	return true;
}

