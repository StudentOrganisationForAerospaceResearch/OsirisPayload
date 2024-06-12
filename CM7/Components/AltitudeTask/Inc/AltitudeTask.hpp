/*
 * AltitudeTask.hpp
 *
 *  Created on: Jun 11, 2024
 *      Author: aiden
 */

#ifndef ALTITUDETASK_HPP_
#define ALTITUDETASK_HPP_


#include "main_system.hpp"
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "AltitudeTask//Apogee-Detection-Everest//MadgwickLibrary//everest.hpp"



class AltitudeTask: public Task
{
public:
	static AltitudeTask& Inst() {
    static AltitudeTask inst;
    return inst;
	}

	void InitTask();

protected:
	static void RunTask(void* pvParams) { AltitudeTask::Inst().Run(pvParams); }

	void Run(void* pvParams);

	void HandleCommand(Command& cm);

private:
	AltitudeTask();        // Private constructor
	AltitudeTask(const AltitudeTask&);                        // Prevent copy-construction
	AltitudeTask& operator=(const AltitudeTask&);


	Everest altFilter;
};





#endif /* ALTITUDETASK_HPP_ */
