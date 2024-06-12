/*
 * Data.hpp
 */

#ifndef DATA_HPP_
#define DATA_HPP_

#include "SystemDefines.hpp"

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
    int32_t altitude;
    int32_t timestamp;
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
