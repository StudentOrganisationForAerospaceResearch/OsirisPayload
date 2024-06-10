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
extern SPI_HandleTypeDef hspi6;
/*
 * @breif Constructor
 */

IMUTask::IMUTask() : Task(IMU_TASK_STACK_DEPTH_WORDS), MainBoardIMU(hi2c2), ExpBoardIMU(hspi6)
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

//	initialize low-level drivers
	while (!MainBoardIMU.init()) {osDelay(30);}
	MainBoardIMU.setInterrupts(false);
	MainBoardIMU.setAccelMax(G4);
	MainBoardIMU.setGyroMax(DPS500);
	MainBoardIMU.setAccelSpeed(IMUTASK_ODR);
	MainBoardIMU.setGyroSpeed(IMUTASK_ODR);
	MainBoardIMU.setAccelOffsetState(true);
	MainBoardIMU.setAccelOffset(0, 0, 0, STRONG); //TODO set these to numbers that make sense

//	GPIO::SPI2_CS::On();
//	while(!ExpBoardIMU.init()) {osDelay(30);}
//	ExpBoardIMU.setInterrupts(false);
//	ExpBoardIMU.setAccelMax(G4);
//	ExpBoardIMU.setGyroMax(DPS500);
//	ExpBoardIMU.setAccelSpeed(IMUTASK_ODR);
//	ExpBoardIMU.setGyroSpeed(IMUTASK_ODR);
//	ExpBoardIMU.setAccelOffsetState(true);
//	ExpBoardIMU.setAccelOffset(0, 0, 0, STRONG); //TODO set these to numbers that make sense

	GPIO::LED_BLUE::On();
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
			 break;
		 case IMU_REQUEST_ANG_ACC:
			 sendAngData(MainBoardIMU);
			 break;
		 default:
				 break;
		 }
		 break;
		 default:
			 break;
	 }
}


void IMUTask::sendLinData(LSM6DSO& unit)
{
	IMUData dataRead;
	unit.readLinearAccel(dataRead.xAccel, dataRead.yAccel, dataRead.zAccel);

	Command IMUData(DATA_COMMAND, OSC_RECEIVE_LINACC); //TODO change this to the correct task and definition
	IMUData.CopyDataToCommand((uint8_t*)&dataRead, sizeof(dataRead));
	FlightTask::Inst().GetEventQueue()->Send(IMUData);
}

void IMUTask::sendAngData(LSM6DSO& unit)
{
	IMUData dataRead;
	unit.readLinearAccel(dataRead.xAccel, dataRead.yAccel, dataRead.zAccel);

	Command IMUData(DATA_COMMAND, OSC_RECEIVE_ANGACC); //TODO Change this to the correct task and definition
	IMUData.CopyDataToCommand((uint8_t*)&dataRead, sizeof(dataRead));
	FlightTask::Inst().GetEventQueue()->Send(IMUData);
}




