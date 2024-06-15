/*
 * MS561101BA03_50.hpp
 *
 *  Created on: Jun 10, 2024
 *      Author: shaan
 */
#include "cstdio"
#include "stm32h7xx_hal.h"
#include "Task.hpp"
#include <cmsis_os.h>
#include <string.h>

#ifndef SENSORS_INC_MS5611_HPP_
#define SENSORS_INC_MS5611_HPP_
#define MS5611_ADDR 0b11101111   //Shifted address for 7-bit I2C address
#define TEST_REG    0x00       // Register address to send test message

class MS5611
{
public:
	MS5611(I2C_HandleTypeDef port);

    /* Initializes the I2C line and resets IMU*/
    bool init(void);

    void readPressure(uint32_t &pressureOut);
    void readTemperature(uint32_t &temperatureOut);

private:
    /*Reads ADC value from barometer*/
    void readADCP();
    void readADCT();

    void readPROM();
    void calculateT(uint32_t &temperatureOut);
    void calculateP(uint32_t &pressureOut);

    /*Reads + converts pressure to digital number, stores in barometer eeprom*/
    void convertT();
    void convertP();

    bool initStatus = false;

    uint8_t reset_data = 0x1E;  // reset CMD
    uint8_t cmdReadADC = 0x00; // read CMD for ADC

    uint8_t osrP;
    uint8_t osrT;

    uint8_t bufP[3];
    uint8_t bufT[3];

    uint8_t delayP;
    uint8_t delayT;

    uint16_t prom[6];

    uint32_t dpv;
    uint32_t dtv;

    uint32_t temperature;
    uint32_t pressure;

    I2C_HandleTypeDef port;
};

#endif /* SENSORS_INC_MS5611_HPP_ */
