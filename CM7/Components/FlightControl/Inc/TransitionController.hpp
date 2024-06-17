/**
 ******************************************************************************
 * File Name          : TransitionController.hpp
 * Description        : Send state transitions based on IMU and Altimeter data
 ******************************************************************************
*/

#include "FlightTask.hpp"
#include "OsirisSM.hpp"
#include "data.hpp"
#include "stdint.h"
#include "Timer.hpp"

class TransitionController {

	static TransitionController& Inst() {
		static TransitionController inst;
		return inst;
	}

	public :
		TransitionController();
		void UpdateStateFromAltitude(LastThreePoints updatedData);
		void UpdateStateFromAccel(float updateVerticalAccel);
		void Setup();

	private:
		inline static void TransitionToLaunch() { FlightTask::Inst().SendCommand(Command(CONTROL_ACTION, OSC_PRELAUNCH_TO_LAUNCH)); }
		inline static void TransitionToDrogue() { FlightTask::Inst().SendCommand(Command(CONTROL_ACTION, OSC_LAUNCH_TO_DROGUE)); }
		inline static void TransitionToMain() { FlightTask::Inst().SendCommand(Command(CONTROL_ACTION, OSC_DROGUE_TO_MAIN)); }
		inline static void TransitionToPostLaunch() { FlightTask::Inst().SendCommand(Command(CONTROL_ACTION, OSC_MAIN_TO_POSTLAUNCH)); }
		static void TransitionToPreLaunch() { FlightTask::Inst().SendCommand(Command(CONTROL_ACTION, OSC_ANY_TO_PRELAUNCH)); }
		static void LaunchTransitionCallback(TimerHandle_t rtTimerHandle);
		bool IsDirectionNegative(LastThreePoints updatedData);
		uint16_t ThreePointBackward(LastThreePoints data);
		Timer* launchTransitionTimer;
};
