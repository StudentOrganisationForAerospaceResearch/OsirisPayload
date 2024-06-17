/**
 ******************************************************************************
 * File Name          : AltimeterTask.cpp
 * Description        : Primary Altimeter task, default task for the system.
 ******************************************************************************
*/
#include "AltimeterTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"
#include "MPL3115A2S_Driver.h"
#include "Data.hpp"
#include "CubeDefines.hpp"
/**
 * @brief Constructor for AltimeterTask
 */
AltimeterTask::AltimeterTask() : Task(ALTIMETER_TASK_QUEUE_DEPTH_OBJS)
{
    data = (AltimeterData*)cube_malloc(sizeof(AltimeterData));
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
    
    MPL3115A2S_Config cfg {0};
    MPL3115A2S_Data_Config dcfg {0};
    MPL3115A2S_Int_Config icfg {0};

    cfg.altimeter_mode = 1;
    cfg.os_ratio = MPL3115A2S_CTRL_OS32;
    dcfg.enable_all_flags = 1;

    MPL3115A2S_Init(&cfg, &dcfg, &icfg);

    while (1) {

        //Process commands in blocking mode
        Command cm;

        if(qEvtQueue->Receive(cm, 100)) {
            HandleCommand(cm);
        }
        else {
            SampleAltimeter();
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
            SampleAltimeter();
            switch(cm.GetTaskCommand()) {
                case ALTIMETER_REQUEST_POLL:
                    SampleAltimeter();
                    break;
                case SEND_TO_ALTITUDE_TO_EVEREST:
                    SendAltitudeToAltimeterFilter();
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

/**
 * @brief This function reads and updates altitude readings from the barometer
 */
void AltimeterTask::SampleAltimeter()
{
    float alt, temp;
    MPL3115A2S_ReadDataPolling(&alt, &temp);

    data->altitude = (int32_t) alt;
    data->timestamp = HAL_GetTick();
}

/**
 * @brief This function sends the latest sample of altitude data to the altimeter filter (Everest)
 * 
 */
void AltimeterTask::SendAltitudeToAltimeterFilter() {
    Command cmd(DATA_COMMAND, (uint16_t)BOWSERTASK_ALTITUDE);
    cmd.CopyDataToCommand((uint8_t*)data, sizeof(AltimeterData));
    Everest::Inst().GetEventQueue()->Send(cmd);
}