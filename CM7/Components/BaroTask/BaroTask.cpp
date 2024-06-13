/**
  ******************************************************************************
  * File Name          : BaroTask.cpp
  *
  *    Source Info     : Based on STMems_Standard_C_drivers Implementation
  *                      lps22hh_STdC/examples/lps22hh_read_data_polling.c
  *
  * Description        : This file contains constants and functions designed to
  *                      obtain accurate pressure and temperature readings from
  *                      the LPS22HH and MS5611 barometers on the Osiris payload.
  *                      A thread task is included that will constantly loop,
  *                      reading and updating the passed BarometerData struct.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "BaroTask.hpp"
#include "main.h"
#include "DebugTask.hpp"
#include "FlightTask.hpp"
#include "CubeDefines.hpp"
// #include "FlashTask.hpp"

/* Macros --------------------------------------------------------------------*/

/* Structs -------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi2;
extern I2C_HandleTypeDef hi2c2;

/* Prototypes ----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
/**
 * @brief Default constructor, sets up storage for member variables
 */
BaroTask::BaroTask() : Task(BARO_TASK_QUEUE_DEPTH_OBJS), mario(hspi2, 0), luigi(hspi2, 1), bowser(hi2c2)
{
  data = (BarometerData*)cube_malloc(sizeof(BarometerData));
}

/**
 * @brief Creates a task for the FreeRTOS Scheduler
 */
void BaroTask::InitTask()
{
  // Make sure the task is not already initialized
  SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize Baro task twice");

  // Start the task
  BaseType_t rtValue =
    xTaskCreate((TaskFunction_t)BaroTask::RunTask,
      (const char*)"BaroTask",
      (uint16_t)BARO_TASK_STACK_DEPTH_WORDS,
      (void*)this,
      (UBaseType_t)BARO_TASK_RTOS_PRIORITY,
      (TaskHandle_t*)&rtTaskHandle);

  //Ensure creation succeeded
  SOAR_ASSERT(rtValue == pdPASS, "BaroTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief BaroTask run loop
 * @param pvParams Currently unused task context
 */
void BaroTask::Run(void * pvParams)
{
  //Initialize the barometers
  mario.init();
  luigi.init();
  bowser.init();
  osDelay(10);

  //Main loop
  while (1) {
    Command cm;
    //Wait forever for a command
    qEvtQueue->ReceiveWait(cm);
    //Process the command
    HandleCommand(cm);
  }
}

/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void BaroTask::HandleCommand(Command& cm)
{
  //TODO: Since this task will stall for a few milliseconds, we may need a way to eat the whole queue (combine similar eg. REQUEST commands and eat to WDG command etc)
  //TODO: Maybe a HandleEvtQueue instead that takes in the whole queue and eats the whole thing in order of non-blocking to blocking

  //Switch for the GLOBAL_COMMAND
  switch (cm.GetCommand()) 
  {
  case REQUEST_COMMAND: 
  {
    HandleRequestCommand(cm.GetTaskCommand());
  }
  case TASK_SPECIFIC_COMMAND: 
  {
    break;
  }
  default:
    SOAR_PRINT("BaroTask - Received Unsupported Command {%d}\n", cm.GetCommand());
    break;
  }

  //No matter what happens, we must reset allocated data
  cm.Reset();
}

/**
 * @brief Handles a Request Command
 * @param taskCommand The command to handle
 */
void BaroTask::HandleRequestCommand(uint16_t taskCommand)
{
  //Switch for task specific command within DATA_COMMAND
  switch (taskCommand) 
  {
  case BARO_REQUEST_NEW_SAMPLE:
    mario.readPressure(data->marioPressure);
    mario.readTemperature(data->marioTemperature);
    luigi.readPressure(data->luigiPressure);
    luigi.readTemperature(data->luigiTemperature);
    bowser.readPressure(data->bowserPressure);
    bowser.readTemperature(data->bowserTemperature);
    break;
  case BARO_REQUEST_FLASH_LOG:
    // LogDataToFlash();
    break;
  case BARO_REQUEST_DEBUG:
	char tx_buffer[50];
    SOAR_PRINT("-- Mario Data (LPS22HH U3) --\n");
    sprintf(tx_buffer, "Pressure (mbar)    : %.2f\r\n", data->marioPressure);
    SOAR_PRINT(tx_buffer);
    sprintf(tx_buffer, "Temperature (degC) : %.2f\r\n", data->marioTemperature);
    SOAR_PRINT(tx_buffer);

    SOAR_PRINT("-- Luigi Data (LPS22HH U4) --\n");
    sprintf(tx_buffer, "Pressure (mbar)    : %.2f\r\n", data->luigiPressure);
    SOAR_PRINT(tx_buffer);
    sprintf(tx_buffer, "Temperature (degC) : %.2f\r\n", data->luigiTemperature);
    SOAR_PRINT(tx_buffer);

    SOAR_PRINT("-- Bowser Data (MS5611 U4) --\n");
    sprintf(tx_buffer, "Pressure (mbar): %.2f\r\n", static_cast<float>(data->bowserPressure) / 100.0);
    SOAR_PRINT(tx_buffer);

    sprintf(tx_buffer, "Temperature (degC): %.2f\r\n", static_cast<float>(data->bowserTemperature) / 100.0);
    SOAR_PRINT(tx_buffer);
    break;
  default:
    SOAR_PRINT("UARTTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
    break;
  }
}

/**
 * @brief Logs barometer data sample to flash
 */
/*
void BaroTask::LogDataToFlash()
{
    Command flashCommand(DATA_COMMAND, WRITE_DATA_TO_FLASH);
    flashCommand.CopyDataToCommand((uint8_t*)data, sizeof(BarometerData));
    FlashTask::Inst().GetEventQueue()->Send(flashCommand);
}
*/

/**
 * @brief Sends the pressure data
 * @param
 */
void BaroTask::sendPressureData(LPS22HH& mario, LPS22HH& luigi, MS5611& bowser)
{
  Command baroData(DATA_COMMAND, OSC_RECEIVE_PRESSURE);
  baroData.AllocateData(sizeof(float));

  mario.readPressure(data->marioPressure);
  baroData.CopyDataToCommand((uint8_t*)&data->marioPressure, sizeof(float));

  luigi.readPressure(data->luigiPressure);
  baroData.CopyDataToCommand((uint8_t*)&data->luigiPressure, sizeof(float));

  bowser.readPressure(data->bowserPressure);
  baroData.CopyDataToCommand((uint8_t*)&data->bowserPressure, sizeof(float));

  FlightTask::Inst().GetEventQueue()->Send(baroData);
}

/**
 *
 * @brief Sends the temperature data
 * @param
 */
void BaroTask::sendTemperatureData(LPS22HH& mario, LPS22HH& luigi, MS5611& bowser)
{
  Command baroData(DATA_COMMAND, OSC_RECEIVE_TEMPERATURE);
  baroData.AllocateData(sizeof(float));

  mario.readTemperature(data->marioTemperature);
  baroData.CopyDataToCommand((uint8_t*)&data->marioTemperature, sizeof(float));

  luigi.readTemperature(data->luigiTemperature);
  baroData.CopyDataToCommand((uint8_t*)&data->luigiTemperature, sizeof(float));

  bowser.readTemperature(data->bowserTemperature);
  baroData.CopyDataToCommand((uint8_t*)&data->bowserTemperature, sizeof(float));

  FlightTask::Inst().GetEventQueue()->Send(baroData);
}
