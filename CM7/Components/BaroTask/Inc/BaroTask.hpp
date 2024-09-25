/**
 ******************************************************************************
 * File Name          : BaroTask.hpp
 * Description        : Retrieves and distributes Barometer data
 ******************************************************************************
*/
#ifndef BAROTASK_INC_BAROTASK_HPP_
#define BAROTASK_INC_BAROTASK_HPP_
/* Includes ------------------------------------------------------------------*/
#include <Data.hpp>
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "main_system.hpp"
#include "LPS22HH.hpp"
#include "MS5611.hpp"

/* Macros/Enums ------------------------------------------------------------*/
enum BARO_COMMANDS {
    BARO_NONE = 0,
    BARO_REQUEST_NEW_SAMPLE,  // Get new barometer samples, task will be blocked for polling time
    BARO_REQUEST_DEBUG,       // Send the current barometer data over the Debug UART
    BARO_REQUEST_FLASH_LOG,   // Log the current barometer data to flash
};

/* Class ------------------------------------------------------------------*/
class BaroTask : public Task
{
public:
    static BaroTask& Inst() {
        static BaroTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void* pvParams) { BaroTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void* pvParams);  // Main run code

    void HandleCommand(Command& cm);
    void HandleRequestCommand(uint16_t taskCommand);

    // void LogDataToFlash();

private:
    BaroTask();                            // Private constructor
    BaroTask(const BaroTask&);             // Prevent copy-construction
    BaroTask& operator=(const BaroTask&);  // Prevent assignment

    void sendPressureData(LPS22HH& mario, LPS22HH& luigi, MS5611& bowser);
    void sendTemperatureData(LPS22HH& mario, LPS22HH& luigi, MS5611& bowser);

    // Private Variables
    LPS22HH mario;
    LPS22HH luigi;
    MS5611 bowser;
    BarometerData* data;
};

#endif /* BAROTASK_INC_BAROTASK_HPP_ */
