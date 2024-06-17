/**
 ******************************************************************************
 * File Name          : AltimeterTask.hpp
 * Description        : Primary flight task, default task for the system.
 ******************************************************************************
*/
#ifndef SOAR_ALTIMETERTASK_HPP_
#define SOAR_ALTIMETERTASK_HPP_
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "Data.hpp"

/* Macros/Enums ------------------------------------------------------------*/

/* Macros/Enums ------------------------------------------------------------*/
enum AltimeterTaskRequests
{
	ALTIMETER_REQUEST_NONE = 0,
	ALTIMETER_REQUEST_POLL,  // Poll Request
    ALTIMETER_REQUEST_DEBUG, // Debug Poll + Print
    SEND_TO_ALTITUDE_TO_EVEREST,
};

class AltimeterTask : public Task
{
public:
    static AltimeterTask& Inst() {
        static AltimeterTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void* pvParams) { AltimeterTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void * pvParams); // Main run code

    void HandleCommand(Command& cm);

    void SampleAltimeter();

    

private:
    // Private Functions
    AltimeterTask();        // Private constructor
    AltimeterTask(const AltimeterTask&);                        // Prevent copy-construction
    AltimeterTask& operator=(const AltimeterTask&);            // Prevent assignment
    void SendAltitudeToAltimeterFilter();

    // Private Variables
    AltimeterData* data;
    uint32_t timestampPT;

};

#endif    // SOAR_ALTIMETERTASK_HPP
