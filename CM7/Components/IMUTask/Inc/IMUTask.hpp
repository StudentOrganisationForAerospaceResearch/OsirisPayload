/*
******************************************************************************
 * File Name          : IMUTask.hpp
 * Description        : Retreives and distributes IMU data
 ******************************************************************************
*/

#ifndef SOAR_IMUTASK_HPP
#define SOAR_IMUTASK_HPP

#include "Sensors//Inc//LSM6DSO.hpp"
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "main_system.hpp"

/* Macros --------------------------------------------------------------------*/

#define IMUTASK_ODR 0b0110 //Page 50 of datasheet to refrence sample rates


/* Enums -----------------------------------------------------------------*/

/* Class -----------------------------------------------------------------*/

class IMUTask: public Task
{
public:
	static IMUTask& Inst() {
	        static IMUTask inst;
	        return inst;
	    }

	void InitTask();

protected:
    static void RunTask(void* pvParams) { IMUTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void * pvParams); // Main run code

    void HandleCommand(Command& cm);

private:
    // Private Functions
       IMUTask();        // Private constructor
       IMUTask(const IMUTask&);                        // Prevent copy-construction
       IMUTask& operator=(const IMUTask&);            // Prevent assignment

    // Private Variables
       I2C_HandleTypeDef hi2c2;
};

#endif //SOAR_IMUTASK_HPP



