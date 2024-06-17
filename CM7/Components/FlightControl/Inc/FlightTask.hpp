/**
 ******************************************************************************
 * File Name          : FlightTask.hpp
 * Description        : Primary flight task, default task for the system.
 ******************************************************************************
*/
#ifndef SOAR_FLIGHTTASK_HPP_
#define SOAR_FLIGHTTASK_HPP_
#include "OsirisSM.hpp"
#include "Task.hpp"
#include "SystemDefines.hpp"

/* Macros/Enums ------------------------------------------------------------*/
enum FlightTaskRequests
{
	FT_REQUEST_NONE = 0,
	FT_REQUEST_TRANSMIT_STATE,
};

class FlightTask : public Task
{
public:
    static FlightTask& Inst() {
        static FlightTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void* pvParams) { FlightTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void * pvParams); // Main run code

    void HandleCommand(Command& cm);

private:
    // Private Functions
    FlightTask();        // Private constructor
    FlightTask(const FlightTask&);                        // Prevent copy-construction
    FlightTask& operator=(const FlightTask&);            // Prevent assignment

    // Private Variables
    OsirisSM* osm_;
};

#endif    // SOAR_FLIGHTTASK_HPP_
