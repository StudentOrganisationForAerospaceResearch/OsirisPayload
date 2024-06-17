/**
 ******************************************************************************
 * File Name          : FlightTask.cpp
 * Description        : Primary flight task, default task for the system.
 ******************************************************************************
*/
#include <OsirisSM.hpp>
#include "FlightTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"

#include "FlashTask//Inc//FlashTask.hpp" //TODO delete me

/**
 * @brief Constructor for FlightTask
 */
FlightTask::FlightTask() : Task(FLIGHT_TASK_QUEUE_DEPTH_OBJS)
{
    osm_ = nullptr;
}

/**
 * @brief Initialize the FlightTask
 */
void FlightTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize flight task twice");

    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)FlightTask::RunTask,
            (const char*)"FlightTask",
            (uint16_t)FLIGHT_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)FLIGHT_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "FlightTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the Flight Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void FlightTask::Run(void * pvParams)
{

	osm_ = new OsirisSM(OS_PRELAUNCH, true);


    while (1) {

        //Process commands in blocking mode
        Command cm;
        bool res = qEvtQueue->ReceiveWait(cm);
        if(res)
            HandleCommand(cm);

    }
}

/**
 * @brief Handle a command from the Command Queue
 * @param cm Command to handle
 */
void FlightTask::HandleCommand(Command& cm)
{
	osm_->HandleCommand(cm);

	// Make sure the command is reset
    cm.Reset();
}
