/**
  ******************************************************************************
  * @file           : SystemDefines.hpp
  * @brief          : Macros and wrappers
  ******************************************************************************
  *
  * Contains system wide macros, defines, and wrappers
  *
  ******************************************************************************
  */
#ifndef CUBE_MAIN_SYSTEM_DEFINES_H
#define CUBE_MAIN_SYSTEM_DEFINES_H

/* Environment Defines ------------------------------------------------------------------*/
//#define COMPUTER_ENVIRONMENT        // Define this if we're in Windows, Linux or Mac (not when flashing on DMB)

#ifdef COMPUTER_ENVIRONMENT
#define __CC_ARM
#endif

/* System Wide Includes ------------------------------------------------------------------*/
#include "main_system.hpp" // C++ Main File Header
#include "UARTDriver.hpp"

/* Cube++ Required Configuration ------------------------------------------------------------------*/
#include "CubeDefines.hpp"
constexpr UARTDriver* const DEFAULT_DEBUG_UART_DRIVER = UART::Debug;    // UART Handle that ASSERT messages are sent over
enum GLOBAL_COMMANDS : uint8_t
{
    COMMAND_NONE = 0,        // No command, packet can probably be ignored
    TASK_SPECIFIC_COMMAND,    // Runs a task specific command when given this object
    DATA_COMMAND,             // Data command, used to send data to a task. Target is stored in taskCommand
	REQUEST_COMMAND,
	CONTROL_ACTION
};

/* Cube++ Optional Code Configuration ------------------------------------------------------------------*/


/* Task Parameter Definitions ------------------------------------------------------------------*/
/* - Lower priority number means lower priority task ---------------------------------*/

// UART TASK
constexpr uint8_t UART_TASK_RTOS_PRIORITY = 2;            // Priority of the uart task
constexpr uint8_t UART_TASK_QUEUE_DEPTH_OBJS = 10;        // Size of the uart task queue
constexpr uint16_t UART_TASK_STACK_DEPTH_WORDS = 512;    // Size of the uart task stack

// DEBUG TASK
constexpr uint8_t TASK_DEBUG_PRIORITY = 2;            // Priority of the debug task
constexpr uint8_t TASK_DEBUG_QUEUE_DEPTH_OBJS = 10;        // Size of the debug task queue
constexpr uint16_t TASK_DEBUG_STACK_DEPTH_WORDS = 512;        // Size of the debug task stack

// FLIGHT TASK
constexpr uint8_t FLIGHT_TASK_RTOS_PRIORITY = 2;            // Priority of the flight task
constexpr uint8_t FLIGHT_TASK_QUEUE_DEPTH_OBJS = 10;        // Size of the flight task queue
constexpr uint16_t FLIGHT_TASK_STACK_DEPTH_WORDS = 512;        // Size of the flight task stack

// IMU TASK
constexpr uint8_t IMU_TASK_RTOS_PRIORITY = 2;            // Priority of the IMU task
constexpr uint8_t IMU_TASK_QUEUE_DEPTH_OBJS = 10;        // Size of the IMU task queue
constexpr uint16_t IMU_TASK_STACK_DEPTH_WORDS = 512;        // Size of the IMU task stack

constexpr uint8_t ALTITUDE_TASK_RTOS_PRIORITY = 2;            // Priority of the Altitude task
constexpr uint8_t ALTITUDE_TASK_QUEUE_DEPTH_OBJS = 10;        // Size of the Altitude task queue
constexpr uint16_t ALTITUDE_TASK_STACK_DEPTH_WORDS = 512;        // Size of the Altitude task stack

// BARO Task
constexpr uint8_t BARO_TASK_RTOS_PRIORITY = 2;            // Priority of the debug task
constexpr uint8_t BARO_TASK_QUEUE_DEPTH_OBJS = 10;        // Size of the debug task queue
constexpr uint16_t BARO_TASK_STACK_DEPTH_WORDS = 512;     // Size of the debug task stack

#endif // CUBE_MAIN_SYSTEM_DEFINES_H
