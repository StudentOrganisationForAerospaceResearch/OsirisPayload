/**
 ******************************************************************************
 * File Name          : TransitionController.cpp
 * Description        : Send state transitions based on IMU and Altimeter data
 ******************************************************************************
*/

#include "TransitionController.hpp"
#include "stdint.h"

TransitionController::TransitionController() {

}

void TransitionController::Setup() {
	launchTransitionTimer = new Timer(LaunchTransitionCallback);
	launchTransitionTimer->ChangePeriodMs(2500);
}

void TransitionController::LaunchTransitionCallback(TimerHandle_t rtTimerHandle) {
//	if(GetMadwickAccel() > 4.0) { MODIFY THIS TO CONFIRM THE ACCELERATION
		TransitionToLaunch();
//	}
}

void TransitionController::UpdateStateFromAltitude(LastThreePoints updatedData) {
	if((updatedData.point1 < 9500.0) && IsDirectionNegative(updatedData)) {
		TransitionToDrogue();
	}
	else if((updatedData.point1 < 1800.0) && IsDirectionNegative(updatedData)) {
		TransitionToMain();
	}
	else if((updatedData.point1 < 300.0) && IsDirectionNegative(updatedData)) {
		TransitionToMain();
	}
}

void TransitionController::UpdateStateFromAccel(float verticalAccel) {
	// Transition to Launch State if acceleration is > 4g for 3s
	if(verticalAccel > 4.0) {
		if(launchTransitionTimer->GetState() == UNINITIALIZED || launchTransitionTimer->GetState() == PAUSED) {
			launchTransitionTimer->Start();
		}
		else if (launchTransitionTimer->GetState() == COUNTING) {
			// Do nothing
		}
	}
}

bool TransitionController::IsDirectionNegative(LastThreePoints updatedData) {
	if (ThreePointBackward(updatedData) < 0 ) {
		return true;
	}

	return false;
}

uint16_t TransitionController::ThreePointBackward(LastThreePoints updatedData) {
	return (updatedData.point3 - 4 * updatedData.point2 + 3 * updatedData.point1);
}

