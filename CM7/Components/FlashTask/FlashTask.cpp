/**
 ******************************************************************************
 * File Name          : FlashTask.cpp
 * Description        : Flash interface task. Used for logging, writing to system
 *                      state, and flash maintenance operations for the system.
 ******************************************************************************
*/
#include "FlashTask.hpp"
#include "GPIO.hpp"
#include "Timer.hpp"
#include "Data.hpp"
#include <cstring>

/**
 * @brief Constructor for FlashTask
 */
FlashTask::FlashTask() : Task(FLASH_TASK_QUEUE_DEPTH_OBJS)
{
}

/**
 * @brief Initialize the FlashTask
 */
void FlashTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize flash task twice");
    
    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)FlashTask::RunTask,
            (const char*)"FlashTask",
            (uint16_t)FLASH_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)FLASH_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "FlashTask::InitTask() - xTaskCreate() failed");

    SOAR_PRINT("Flash Task initialized");
}

/**
 * @brief Instance Run loop for the Flash Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void FlashTask::Run(void * pvParams)
{
    // Wait until the flash has been initialized by flight task
    while (flashChipHandle.init() != FLASH_OK) {
      osDelay(10);
    }
    while (1) {
    Command cm;
		//Wait forever for a command
		qEvtQueue->ReceiveWait(cm);
		//Process the command
		HandleCommand(cm);
    }
}

/**
 * @brief Handles current command
 * @param cm The command to handle
 */
void FlashTask ::HandleCommand(Command& cm)
{
    switch (cm.GetCommand()) {
    case TASK_SPECIFIC_COMMAND: {
        if (cm.GetTaskCommand() == DUMP_FLASH_DATA)
        {
            ReadLogDataFromFlash();
        }
        else if (cm.GetTaskCommand() == ERASE_ALL_FLASH)
        {
            // Erase the chip
            flashChipHandle.eraseChip();
			currentOffsets_.writeDataOffset = 0;
        }
        else 
        {
            SOAR_PRINT("FlashTask Received Unsupported Task Command: %d\n", cm.GetTaskCommand());
        }
        break;
    }
    case DATA_COMMAND: {
        // If the command is not a WRITE_DATA_TO_FLASH command do nothing
        if (cm.GetTaskCommand() != WRITE_DATA_TO_FLASH) {
            SOAR_PRINT("FlashTask Received Unsupported Data Command: %d\n", cm.GetTaskCommand());
            break;
        }
        WriteLogDataToFlash(cm.GetDataPointer(), cm.GetDataSize());
        break;
    }
    default:
        SOAR_PRINT("FlashTask Received Unsupported Command: %d\n", cm.GetCommand());
        break;
    }
    cm.Reset();
}

/**
 * @brief writes data to flash with the size of the data written as the header, increases offset by size + 1 to account for size, currently only handles size < 255
 */
void FlashTask::WriteLogDataToFlash(uint8_t* data, uint16_t size)
{
    uint8_t buff[size + 1];

    buff[0] = (uint8_t)(size & 0xff);
    memcpy(buff + 1, data, size);

	flashChipHandle.write(data, SPI_FLASH_LOGGING_STORAGE_START_ADDR + currentOffsets_.writeDataOffset, size + 1);
    currentOffsets_.writeDataOffset += size + 1;

    //TODO: Consider adding a readback to check if it was successful
}

/**
 * @brief reads all data and prints it through UART up until offset read from struct
 *        currently unimplemented
 */
bool FlashTask::ReadLogDataFromFlash()
{
    //unused
    bool res = true;

    uint8_t length;

    for (unsigned int i = 0; i < currentOffsets_.writeDataOffset + SPI_FLASH_LOGGING_STORAGE_START_ADDR; i++) {
    	flashChipHandle.read(&length, SPI_FLASH_LOGGING_STORAGE_START_ADDR + i, sizeof(length));

        if (length == sizeof(IMUData)) {
            uint8_t dataRead[sizeof(IMUData)];
            flashChipHandle.read(dataRead, SPI_FLASH_LOGGING_STORAGE_START_ADDR + i + 1, sizeof(IMUData));
            IMUData* IMURead = (IMUData*)dataRead;
            SOAR_PRINT("length: %d, xAccel: %f, yAccel: %f, zAccel: %f\n",
                length, IMURead->xAccel, IMURead->yAccel, IMURead->zAccel);
        }
		else if (length == sizeof(AltimeterData)) {
            uint8_t dataRead[sizeof(AltimeterData)];
            flashChipHandle.read(dataRead, SPI_FLASH_LOGGING_STORAGE_START_ADDR + i + 1, sizeof(AltimeterData));
            AltimeterData* altimeterRead = (AltimeterData*)dataRead;
            SOAR_PRINT("length: %d, altitude: %f, temp: %f\n",
                length, altimeterRead->altitude, altimeterRead->temp);
		}
        else if (length == sizeof(BarometerData)) {
            uint8_t dataRead[sizeof(BarometerData)];
            flashChipHandle.read(dataRead, SPI_FLASH_LOGGING_STORAGE_START_ADDR + i + 1, sizeof(BarometerData));
            BarometerData* baroRead = (BarometerData*)dataRead;
            SOAR_PRINT("length: %d, marioPressure: %f, luigiPressure: %f, bowserPressure: %f\n marioTemperature, luigiTemperature, bowserTemperature: %f\n",
            		length, baroRead->marioPressure, baroRead->luigiPressure, baroRead->bowserPressure,
					baroRead->marioTemperature, baroRead->luigiTemperature, baroRead->bowserTemperature);
        }
        else {
            SOAR_PRINT("Unknown length, readback brokedown: %d\n", length);
        }
        i = i + length;
    }
    return res;
}
