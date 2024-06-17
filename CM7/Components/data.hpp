/*
 * Data.hpp
 */

#ifndef DATA_HPP_
#define DATA_HPP_

#include "stdint.h"
#include "SystemDefines.hpp"

typedef struct IMUData {
	float xAccel;
	float yAccel;
	float zAccel;
} IMUData;

typedef struct AltimeterData {
	float altitude;
	float temp;
} AltimeterData;

/*
 *  mario  = Experiment Board LPS22HH U3
 *  luigi  = Experiment Board LPS22HH U4
 *  bowser = Main Board MS5611 U4
 */
typedef struct BarometerData {
    float marioPressure;
    float marioTemperature;
    float luigiPressure;
    float luigiTemperature;
    // uint32_t bowserPressure;
    // uint32_t bowserTemperature;
} BarometerData;

typedef struct AltimeterData {
    uint32_t altitude;  // scale the the data by 100 when sending to achieve a 2 decimal point accuracy
    uint32_t timestamp;
} AltimeterData;

typedef struct BarometerAltitudeData {
    uint32_t marioAltitude;
    uint32_t luigiAltitude;
}

/*
 * Data Containers
 * Acts as a pointer to the other data structs alongside a timestamp for logging to flash.
 */
typedef struct AllData {
	// IMUData*         	 imuData;
	// AltimeterData* 		 altimeterData;
    BarometerData*       barometerData;
    uint32_t 			 time;
} AllData;
// Used to send the 3 newest altitude data points
typedef struct LastThreePoints {
	uint16_t point1;
	uint16_t point2;
	uint16_t point3;
} LastThreePoints;

#endif /* DATA_HPP_ */
