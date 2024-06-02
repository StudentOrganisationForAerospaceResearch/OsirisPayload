/**
 ******************************************************************************
 * File Name          : IMUTask.cpp
 * Description        : Retreives and distributes IMU data
 ******************************************************************************
*/
#include "Inc//IMUTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"


/*
 * @breif Constructor
 */

IMUTask::IMUTask() : Task(IMU_TASK_STACK_DEPTH_WORDS)
{
}

/*
 * @breif Initialize task
 */

void IMUTask::InitTask()
{
	// Make sure the task is not already initialized
	    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize flight task twice");

	    // Make sure the task is not already initialized
	        SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize flight task twice");

	        BaseType_t rtValue =
	            xTaskCreate((TaskFunction_t)IMUTask::RunTask,
	                (const char*)"IMUTask",
	                (uint16_t)IMU_TASK_STACK_DEPTH_WORDS,
	                (void*)this,
	                (UBaseType_t)IMU_TASK_RTOS_PRIORITY,
	                (TaskHandle_t*)&rtTaskHandle);

	        SOAR_ASSERT(rtValue == pdPASS, "IMUTask::InitTask() - xTaskCreate() failed");
}

void IMUTask::Run(void *pvParams)
{
	//initialize low-level drivers
	LSM6DSO MainBoardIMU(hi2c2);

	while (!MainBoardIMU.init()) {}
	MainBoardIMU.setInterrupts(false);
	MainBoardIMU.setAccelMax(G4);



}





