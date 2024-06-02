/**
 ******************************************************************************
 * File Name          : AltimeterTask.cpp
 * Description        : Primary Altimeter task, default task for the system.
 ******************************************************************************
*/
#include "AltimeterTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"

/**
 * @brief Constructor for AltimeterTask
 */
AltimeterTask::AltimeterTask() : Task(ALTIMETER_TASK_QUEUE_DEPTH_OBJS)
{
}

/**
 * @brief Initialize the AltimeterTask
 */
void AltimeterTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize Altimeter task twice");

    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)AltimeterTask::RunTask,
            (const char*)"AltimeterTask",
            (uint16_t)ALTIMETER_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)ALTIMETER_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "AltimeterTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the Altimeter Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void AltimeterTask::Run(void * pvParams)
{
    
    while (1) {

        //Process commands in blocking mode
        Command cm;
        bool res = qEvtQueue->ReceiveWait(cm);
        if(res) {
            HandleCommand(cm);
        }
    }
}

/**
 * @brief Handle a command from the Command Queue
 * @param cm Command to handle
 */
void AltimeterTask::HandleCommand(Command& cm)
{
    switch(cm.GetCommand()) {
        case REQUEST_COMMAND:
            // Poll the altimeter
            switch(cm.GetTaskCommand()) {
                case ALTIMETER_REQUEST_POLL:
                    // Poll the altimeter
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
	// Make sure the command is reset
    cm.Reset();
}
