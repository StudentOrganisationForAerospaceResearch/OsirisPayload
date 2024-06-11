/*
 * LPS22HH.hpp
 *
 *  Created on: Jun 1, 2024
 *      Author: Javy
 */

#ifndef SENSORS_INC_LPS22HH_HPP_
#define SENSORS_INC_LPS22HH_HPP_

#include "string.h"
#include "main_system.hpp"
#include "lps22hh_reg.h"
#include "main.h"

#define data_polling
//#define data_drdy
//#define fifo_irq

#define BOOT_TIME 300 //ms

typedef union {
  int16_t i16bit;
  uint8_t u8bit[2];
} axis1bit16_t;

typedef union {
  uint32_t u32bit;
  uint8_t u8bit[4];
} axis1bit32_t;

typedef union {
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

class LPS22HH {
private:
	SPI_HandleTypeDef spi;

	uint8_t whoamI, rst;
	stmdev_ctx_t lps22hhDriver;
	lps22hh_reg_t reg_LPS22HH;

	#if defined(fifo_irq) || defined(data_drdy)
		lps22hh_pin_int_route_t int_route;
	#endif

	#if defined(data_drdy)
		uint8_t drdy_event;
	#elif defined(fifo_irq)
		uint8_t fifo_wtm_event;
	#endif

	bool initStatus = false;
	bool unitSelect = 0; // 0 = MARIO, 1 = LUIGI
	// bool usingInterrupts = false;

	static int32_t readReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
	static int32_t writeReg(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
	static void initDelay(uint32_t ms);

public:
	LPS22HH(SPI_HandleTypeDef &spi, bool unitSelect);

	// Returns True on Success
	bool init(void);
	bool readPressure(float &pressureOut);
	bool readTemperature(float &temperatureOut);
	
  // bool setInterrupts(bool useInterrupts);
};

#endif /* SENSORS_INC_LPS22HH_HPP_ */
