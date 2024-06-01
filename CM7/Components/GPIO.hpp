/**
 ******************************************************************************
 * File Name          : GPIO.hpp
 * Description        : Handles Controls for GPIO Pins
 *
 *    GPIO contains all GPIO pins wrapped in a namespace and corresponding functions
 *
 *    All GPIO pins should be controlled through this abstraction layer to ensure readable control.
 *
 ******************************************************************************
*/
#ifndef PAYLOAD_INCLUDE_SOAR_CORE_GPIO_H
#define PAYLOAD_INCLUDE_SOAR_CORE_GPIO_H
#include "SystemDefines.hpp"
#include "main.h"
#include "stm32h7xx_hal.h"

namespace GPIO
{
	namespace LED_GREEN {
		inline void On() { HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET); }
		inline void Off() { HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET); }
		inline void Toggle() { HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin); }

		inline bool IsOn() { return HAL_GPIO_ReadPin(LED_GREEN_GPIO_Port, LED_GREEN_Pin) == GPIO_PIN_SET; }
	}

	namespace LED_BLUE {
			inline void On() { HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET); }
	        inline void Off() { HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET); }
	        inline void Toggle() { HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin); }

	        inline bool IsOn() { return HAL_GPIO_ReadPin(LED_BLUE_GPIO_Port, LED_BLUE_Pin) == GPIO_PIN_SET; }
	}
}

#endif /* PAYLOAD_INCLUDE_SOAR_CORE_GPIO_H */
