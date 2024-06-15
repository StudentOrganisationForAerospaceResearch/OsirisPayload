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

IMUTask::IMUTask() : Task(IMU_TASK_STACK_DEPTH_WORDS), toopy(hi2c2), binoo(hspi6)
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
//	while (!toopy.init()) {osDelay(30);}
//	toopy.setInterrupts(false);
//	toopy.setAccelMax(G4);
//	toopy.setGyroMax(DPS500);
//	toopy.setAccelSpeed(IMUTASK_ODR);
//	toopy.setGyroSpeed(IMUTASK_ODR);
//	toopy.setAccelOffsetState(true);
//	toopy.setAccelOffset(0, 0, 0, STRONG); //TODO set these to numbers that make sense

	GPIO::SPI2_CS::On();
	while(!binoo.init()) {osDelay(30);}
	binoo.setInterrupts(false);
	binoo.setAccelMax(G4);
	binoo.setGyroMax(DPS500);
	binoo.setAccelSpeed(IMUTASK_ODR);
	binoo.setGyroSpeed(IMUTASK_ODR);
	binoo.setAccelOffsetState(true);
	binoo.setAccelOffset(0, 0, 0, STRONG); //TODO set these to numbers that make sense


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
			 sendLinData(toopy);
			 break;
		 case IMU_REQUEST_ANG_ACC:
			 sendAngData(toopy);
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




