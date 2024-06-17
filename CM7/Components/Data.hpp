/*
 * Data.hpp
 *
 */

#ifndef DATA_HPP_
#define DATA_HPP_


#include <stdint.h>
#include "SystemDefines.hpp"

typedef union logFloat {
	float f;
	uint8_t u8[4];
	uint32_t u32;
} logFloat;

typedef struct IMUData {
	logFloat xAccel;
	logFloat yAccel;
	logFloat zAccel;
} IMUData;

typedef struct AltimeterData {
	logFloat altitude;
	logFloat temp;
} AltimeterData;

/*
 *  mario  = Experiment Board LPS22HH U3
 *  luigi  = Experiment Board LPS22HH U4
 *  bowser = Main Board MS5611 U4
 */
typedef struct BarometerData {
    logFloat marioPressure;
    logFloat marioTemperature;
    logFloat luigiPressure;
    logFloat luigiTemperature;
    logFloat bowserPressure;
    logFloat bowserTemperature;
} BarometerData;

/*
 * Data Containers
 * Acts as a pointer to the other data structs alongside a timestamp for logging to flash.
 */
typedef struct AllData {
	 IMUData*         	 imuData;
	 AltimeterData* 	altimeterData;
	 BarometerData*       barometerData;
	 logFloat 			 time;
} AllData;

#endif /* DATA_HPP_ */
