/**
 ******************************************************************************
 * File Name          : IMUTask.cpp
 * Description        : Retreives and distributes IMU data
 ******************************************************************************
*/
#include "Inc//IMUTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"
#include "FlightTask.hpp"
#include "Data.hpp"

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
//	Command test(DATA_COMMAND, OSC_RECEIVE_LINACC);
//	IMUData testData;
//	testData.xAccel = 0xAAAA;
//	testData.yAccel = 0xFFFF;
//	testData.zAccel = 0xBBBB;
//	test.CopyDataToCommand((uint8_t*)&testData, sizeof(testData));
//	FlightTask::Inst().GetEventQueue()->Send(test);

//	initialize low-level drivers
	while (!MainBoardIMU.init()) {osDelay(30);}
	MainBoardIMU.setInterrupts(false);
	MainBoardIMU.setAccelMax(G4);
	MainBoardIMU.setGyroMax(DPS500);
	MainBoardIMU.setAccelSpeed(IMUTASK_ODR);
	MainBoardIMU.setAccelSpeed(IMUTASK_ODR);
	MainBoardIMU.setAccelOffsetState(true);
	MainBoardIMU.setAccelOffset(1, 1, 1, STRONG); //TODO set these to numbers that make sense

//	sendLinData(MainBoardIMU);

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
		 case IMU_REQUEST_LIN_ACC:
			 sendLinData(MainBoardIMU);
		 case IMU_REQUEST_ANG_ACC:
			 sendAngData(MainBoardIMU);
		 default:
				 break;
		 }
		 default:
			 break;
	 }
}


void IMUTask::sendLinData(LSM6DSO& unit)
{
	unit.readLinearAccel(buffer[0], buffer[2], buffer[4]);

	char printBuffer[31];
	sprintf(printBuffer, " X: %u\n Y: %u\n Z: %u\n", buffer[0], buffer[2], buffer[4]);
	SOAR_PRINT(printBuffer);

	Command IMUData(DATA_COMMAND, OSC_RECEIVE_LINACC);
	IMUData.AllocateData(6);
	for (uint8_t i = 0; i < 6; i+2)
	{
	 buffer[i+1] = buffer[i] && 0x00FF;
	 buffer[i] = buffer[i] >> 8;
	}
	IMUData.CopyDataToCommand((uint8_t*)buffer, 6); //TODO call convertToDPS() on the other side
	FlightTask::Inst().GetEventQueue()->Send(IMUData);
}

void IMUTask::sendAngData(LSM6DSO& unit)
{
	unit.readAngularAccel(buffer[0], buffer[2], buffer[4]);
	Command IMUData(DATA_COMMAND, OSC_RECEIVE_ANGACC);
	IMUData.AllocateData(6);
	for (uint8_t i = 0; i < 6; i+2)
	{
	 buffer[i+1] = buffer[i] && 0x00FF;
	 buffer[i] = buffer[i] >> 8;
	}
	IMUData.CopyDataToCommand((uint8_t*)buffer, 6); //TODO call convertToDPS() on the other side
	FlightTask ::Inst().GetEventQueue()->Send(IMUData);
}




