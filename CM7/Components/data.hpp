/*
 * Data.hpp
 */

#ifndef DATA_HPP_
#define DATA_HPP_

#include "SystemDefines.hpp"
#include "stdint.h"

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
    // float bowserPressure;
    // float bowserTemperature;
} BarometerData;

typedef struct AltimeterData {
    uint32_t altitude;  // scale the the data by 100 when sending to achieve a 2 decimal point accuracy
    uint32_t timestamp;
} AltimeterData;

/*
 * Data Containers
 * Acts as a pointer to the other data structs alongside a timestamp for logging to flash.
 */
typedef struct AllData {
    // IMUData*              imuData;
    BarometerData*       barometerData;
    AltimeterData*       altimeterData;
    uint32_t             time;
} AllData;

#endif /* DATA_HPP_ */
