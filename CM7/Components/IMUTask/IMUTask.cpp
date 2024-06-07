/**
 ******************************************************************************
 * File Name          : IMUTask.cpp
 * Description        : Retreives and distributes IMU data
 ******************************************************************************
*/
#include "Inc//IMUTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"

extern I2C_HandleTypeDef hi2c2;

/*
 * @breif Constructor
 */

IMUTask::IMUTask() : Task(IMU_TASK_STACK_DEPTH_WORDS), MainBoardIMU(hi2c2)
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
	while (!MainBoardIMU.init()) {osDelay(30);}
	MainBoardIMU.setInterrupts(false);
	MainBoardIMU.setAccelMax(G4);
	MainBoardIMU.setGyroMax(DPS500);
	MainBoardIMU.setAccelSpeed(IMUTASK_ODR);
	MainBoardIMU.setAccelSpeed(IMUTASK_ODR);
	MainBoardIMU.setAccelOffsetState(true);
	MainBoardIMU.setAccelOffset(1, 1, 1, STRONG); //TODO set these to numbers that make sense

	//	LSM6DSO ExpBoardIMU(hi2c)
	//	while(!ExpBoardIMU.init())

	while (1) {

		//Process commands in blocking mode
		Command cm;
		bool res = qEvtQueue->ReceiveWait(cm);
		if(res)
			HandleCommand(cm);
	}
}

void IMUTask::HandleCommand(Command& com)
{
	 switch(com.GetCommand())
	 {
	 case REQUEST_COMMAND:
		 switch(com.GetTaskCommand())
		 {
		 case READACC:
			 MainBoardIMU.readLinearAccel(buffer[0], buffer[2], buffer[4]);
			 Command IMUData(DATA_COMMAND, FlightTask::Inst());
			 IMUData.AllocateData(6);
			 for (uint8_t i = 0; i < 3; i++)
			 {
				 buffer[i+1] = buffer[i] && 0x00FF;
				 buffer[i] = buffer[i] >> 8;
			 }
			 IMUData.CopyDataToCommand((uint8_t*)buffer, 6); //TODO call convertToM2() on the other side
		 case READGYR:
			 MainBoardIMU.readAngularAccel(buffer[0], buffer[2], buffer[4]);
			 Command IMUData(DATA_COMMAND, FlightTask::Inst());
			 IMUData.AllocateData(6);
			 for (uint8_t i = 0; i < 3; i++)
			 {
				 buffer[i+1] = buffer[i] && 0x00FF;
				 buffer[i] = buffer[i] >> 8;
			 }
			 IMUData.CopyDataToCommand((uint8_t*)buffer, 6); //TODO call convertToDPS() on the other side
		 }
		 default:
			 break;
	 }
}




