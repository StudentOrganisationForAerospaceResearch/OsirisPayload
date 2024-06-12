/*
 * AltitudeTask.cpp
 *
 *  Created on: Jun 11, 2024
 *      Author: aiden
 */

#include "Inc//AltitudeTask.hpp"

AltitudeTask::AltitudeTask() : Task(ALTITUDE_TASK_QUEUE_DEPTH_OBJS)
{

}

void AltitudeTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize flight task twice");

    BaseType_t rtValue =
    		xTaskCreate((TaskFunction_t)AltitudeTask::RunTask,
    				(const char*)"AltitudeTask",
					(uint16_t)ALTITUDE_TASK_STACK_DEPTH_WORDS,
					(void*)this,
					(UBaseType_t)ALTITUDE_TASK_RTOS_PRIORITY,
					(TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "AltitudeTask::InitTask() - xTaskCreate() failed");
}

void AltitudeTask::Run(void* pvParams)
{

	MadgwickSetup();

	while (1) {
		//Process commands in blocking mode
		Command cm;
		bool res = qEvtQueue->ReceiveWait(cm);
		if(res)
			HandleCommand(cm);
	}
}


void AltitudeTask::HandleCommand(Command& cm)
{
//	switch()
}

