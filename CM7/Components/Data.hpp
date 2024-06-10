/*
 * Data.hpp
 */

#ifndef DATA_HPP_
#define DATA_HPP_
#include <stdint.h>

typedef struct IMUData {
	float xAccel;
	float yAccel;
	float zAccel;
} IMUData;

typedef struct AltimeterData {
	float altitude;
	float temp;
} AltimeterData;

#endif /* DATA_HPP_ */
