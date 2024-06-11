/*
 * LPS22HH.cpp
 *
 *  Created on: Jun 1, 2024
 *      Author: Javy
 */

#include "LPS22HH.hpp"
#include "GPIO.hpp"

LPS22HH::LPS22HH(SPI_HandleTypeDef &spi, bool unitSelect) : spi(spi), unitSelect(unitSelect)
{
}

bool LPS22HH::init(void)
{
  // Initialize mems driver interface
  lps22hhDriver.handle 	   = &spi;
  lps22hhDriver.write_reg  = &LPS22HH::writeReg;
  lps22hhDriver.read_reg   = &LPS22HH::readReg;
  lps22hhDriver.mdelay 	   = &LPS22HH::initDelay;

  initDelay(BOOT_TIME);

  // Check device ID
  while (whoamI != LPS22HH_ID) {
	SOAR_PRINT("WHO_AM_I: 0x%02X\n", whoamI);
	osDelay(300);
    lps22hh_reset_set(&lps22hhDriver, PROPERTY_ENABLE);
	do {
	  lps22hh_reset_get(&lps22hhDriver, &rst);
	} while (rst);
	osDelay(300);
	lps22hh_device_id_get(&lps22hhDriver, &whoamI);
  }
  SOAR_PRINT("WHO_AM_I: 0x%02X\n", whoamI);
  initStatus = true;

  /* Restore default configuration */
  lps22hh_reset_set(&lps22hhDriver, PROPERTY_ENABLE);
  do {
	lps22hh_reset_get(&lps22hhDriver, &rst);
  } while (rst);

  // Configure data acquisition mode (polling, drdy, fifo) 
  #if defined(data_polling)
    lps22hh_block_data_update_set(&lps22hhDriver, PROPERTY_ENABLE);
    lps22hh_data_rate_set(&lps22hhDriver, LPS22HH_10_Hz_LOW_NOISE); // ODR
  #elif defined(data_drdy)
    lps22hh_int_notification_set(&lps22hhDriver, LPS22HH_INT_PULSED);
    lps22hh_pin_int_route_get(&lps22hhDriver, &int_route);
    int_route.drdy_pres = PROPERTY_ENABLE;
    lps22hh_pin_int_route_set(&lps22hhDriver, &int_route);
    lps22hh_block_data_update_set(&lps22hhDriver, PROPERTY_ENABLE);
    lps22hh_data_rate_set(&lps22hhDriver, LPS22HH_10_Hz_LOW_NOISE); // ODR
  #elif defined(fifo_irq)
    lps22hh_fifo_watermark_set(&lps22hhDriver, 32); // FIFO watermark level
    lps22hh_fifo_stop_on_wtm_set(&lps22hhDriver, PROPERTY_ENABLE);
    lps22hh_fifo_mode_set(&lps22hhDriver, LPS22HH_STREAM_MODE);
    lps22hh_int_notification_set(&lps22hhDriver, LPS22HH_INT_LATCHED);
    lps22hh_pin_int_route_get(&lps22hhDriver, &int_route);
    int_route.fifo_th = PROPERTY_ENABLE;
    lps22hh_pin_int_route_set(&lps22hhDriver, &int_route);
    lps22hh_data_rate_set(&lps22hhDriver, LPS22HH_10_Hz); // ODR
  #endif

	return initStatus;
}

int32_t LPS22HH::readReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
	LPS22HH* self = static_cast<LPS22HH*>(handle);
	int32_t retVal = 0;
	uint8_t regVal = (reg | 0x80);
	if (self->unitSelect == 0) { 		 // MARIO
		GPIO::MARIO::Reset();
	} else if (self->unitSelect == 1) {  // LUIGI
		GPIO::LUIGI::Reset();
	}
	HAL_SPI_Transmit(static_cast<SPI_HandleTypeDef*>(self->lps22hhDriver.handle), &regVal, 1, 1000);
	retVal = HAL_SPI_Receive(static_cast<SPI_HandleTypeDef*>(self->lps22hhDriver.handle), bufp, len, 1000);
	if (self->unitSelect == 0) { 		 // MARIO
		GPIO::MARIO::Set();
	} else if (self->unitSelect == 1) {  // LUIGI
		GPIO::LUIGI::Set();
	}
	return retVal;
}

int32_t LPS22HH::writeReg(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
	LPS22HH* self = static_cast<LPS22HH*>(handle);
	int32_t retVal = 0;
	if (self->unitSelect == 0) { 		 // MARIO
		GPIO::MARIO::Reset();
	} else if (self->unitSelect == 1) {  // LUIGI
		GPIO::LUIGI::Reset();
	}
	HAL_SPI_Transmit(static_cast<SPI_HandleTypeDef*>(self->lps22hhDriver.handle), &reg, 1, 1000);
	retVal = HAL_SPI_Transmit(static_cast<SPI_HandleTypeDef*>(self->lps22hhDriver.handle), bufp, len, 1000);
	if (self->unitSelect == 0) { 		 // MARIO
		GPIO::MARIO::Set();
	} else if (self->unitSelect == 1) {  // LUIGI
		GPIO::LUIGI::Set();
	}
	return retVal;
}

bool LPS22HH::readPressure(float &pressureOut)
{
	if (initStatus == false) return false;
	axis1bit32_t pressureRaw;
	float pressureFloat = 0; // hPa

  // Read samples on event
  #if defined(data_polling)
  	lps22hh_read_reg(&lps22hhDriver, LPS22HH_STATUS, (uint8_t *)&reg_LPS22HH, 1);
		//if (reg_LPS22HH.status.p_da) {
			memset(pressureRaw.u8bit, 0x00, sizeof(uint32_t));
			lps22hh_pressure_raw_get(&lps22hhDriver, &pressureRaw.u32bit);
			pressureFloat = lps22hh_from_lsb_to_hpa(pressureRaw.u32bit);
		//}

  #elif defined(data_drdy)
    if (drdy_event) {
      uint8_t p_drdy;
      drdy_event = 0;
      // Read output only if new value is available
      lps22hh_press_flag_data_ready_get(&lps22hhDriver, &p_drdy);
      if (p_drdy) {
        memset(&pressureRaw.u8bit, 0x00, sizeof(uint32_t));
        lps22hh_pressure_raw_get(&lps22hhDriver, &pressureRaw.u8bit);
        pressureFloat = lps22hh_from_lsb_to_hpa( pressureRaw.i32bit);
      }
    }

  #elif defined(fifo_irq)
		// Read output only if interrupt is active
    if (fifo_wtm_event == 1) {
      uint8_t wtm_flag, num, i;
      fifo_wtm_event = 0;
      lps22hh_fifo_wtm_flag_get(&dev_ctx, &wtm_flag);
      if (wtm_flag) {
        lps22hh_fifo_data_level_get(&dev_ctx, &num);
        for (i = 0; i < num; i++) {
          lps22hh_fifo_pressure_raw_get(&dev_ctx, &pressureRaw);
          pressureFloat = lps22hh_from_lsb_to_hpa(pressureRaw);
        }
      }
    }
	#endif

	pressureOut = pressureFloat;
	return true;
}

bool LPS22HH::readTemperature(float &temperatureOut)
{
	if (initStatus == false) return false;
	axis1bit16_t temperatureRaw;
	float temperatureFloat = 0; // degC

	// Read samples on event
  #if defined(data_polling)
  	// Read output only if new value is available
  	lps22hh_read_reg(&lps22hhDriver, LPS22HH_STATUS, (uint8_t *)&reg_LPS22HH, 1);
		//if (reg_LPS22HH.status.t_da) {
			memset(temperatureRaw.u8bit, 0x00, sizeof(int16_t));
			lps22hh_temperature_raw_get(&lps22hhDriver, &temperatureRaw.i16bit);
			temperatureFloat = lps22hh_from_lsb_to_celsius(temperatureRaw.i16bit);
		//}

  #elif defined(data_drdy)
    if (drdy_event) {
      uint8_t t_drdy;
      drdy_event = 0;
      // Read output only if new value is available
      lps22hh_temp_flag_data_ready_get(&lps22hhDriver, &t_drdy);
			if (t_drdy) {
				memset(&temperatureRaw.u8bit, 0x00, sizeof(int16_t));
				lps22hh_temperature_raw_get(&lps22hhDriver, &temperatureRaw.u8bit);
				temperatureFloat = lps22hh_from_lsb_to_celsius(temperatureRaw.i16bit );
			}
    }

  #elif defined(fifo_irq)
		// Read output only if interrupt is active
    if (fifo_wtm_event == 1) {
        uint8_t wtm_flag, num, i;
        fifo_wtm_event = 0;
        lps22hh_fifo_wtm_flag_get(&dev_ctx, &wtm_flag);
        if (wtm_flag) {
          lps22hh_fifo_data_level_get(&dev_ctx, &num);
          for (i = 0; i < num; i++) {
            lps22hh_fifo_temperature_raw_get(&lps22hhDriver, &temperatureRaw.u8bit);
            temperatureFloat = lps22hh_from_lsb_to_celsius(temperatureRaw.i16bit);
          }
        }
    }
	#endif

	temperatureOut = temperatureFloat;
	return true;
}

void LPS22HH::initDelay(uint32_t ms)
{
	osDelay(ms);
}
