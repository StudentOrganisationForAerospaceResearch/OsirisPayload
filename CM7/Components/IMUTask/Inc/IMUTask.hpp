/*
******************************************************************************
 * File Name          : IMUTask.hpp
 * Description        : Retreives and distributes IMU data
 ******************************************************************************
*/

#define toopy MainBoardIMU
#define binoo ExpBoardIMU


#ifndef SOAR_IMUTASK_HPP
#define SOAR_IMUTASK_HPP

#include "Sensors//Inc//LSM6DSO.hpp"
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "main_system.hpp"
//#include "main.c"

/* Macros --------------------------------------------------------------------*/

#define IMUTASK_ODR 0b0110 //Page 50 of datasheet to refrence sample rates





/* Enums -----------------------------------------------------------------*/

enum IMU_COMMANDS {
	IMU_REQUEST_LIN_ACC,
	IMU_REQUEST_ANG_ACC,
};

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

    void HandleCommand(Command& com);

private:
    // Private Functions
       IMUTask();        // Private constructor
       IMUTask(const IMUTask&);                        // Prevent copy-construction
       IMUTask& operator=(const IMUTask&);            // Prevent assignment

       void sendLinData(LSM6DSO& unit);
       void sendAngData(LSM6DSO& unit);

    // Private Variables
       LSM6DSO MainBoardIMU;
       LSM6DSO ExpBoardIMU;
};

#endif //SOAR_IMUTASK_HPP



