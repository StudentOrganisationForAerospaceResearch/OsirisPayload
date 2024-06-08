/*
 * Data.hpp
 */

#ifndef DATA_HPP_
#define DATA_HPP_
#include <stdint.h>

typedef struct IMUData {
	uint16_t xAccel;
	uint16_t yAccel;
	uint16_t zAccel;
} IMUData;

typedef struct AltimeterData {
	float altitude;
	float temp;
} AltimeterData;

#endif /* DATA_HPP_ */
