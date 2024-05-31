/**
 ******************************************************************************
 * File Name          : OsirisSM.cpp
 * Description        : Primary rocket state machine
 ******************************************************************************
*/
#include <OsirisSM.hpp>
#include "SystemDefines.hpp"
#include "GPIO.hpp"

/* Rocket State Machine ------------------------------------------------------------------*/

/**
 * @brief Default constructor for Rocket SM, initializes all states
 */
OsirisSM::OsirisSM(OsirisState startingState, bool enterStartingState)
{
	// Setup of internal state array
	stateArray[OS_PRELAUNCH] = new PreLaunch();
	stateArray[OS_LAUNCH] = new Launch();
	stateArray[OS_DROGUE] = new Drogue();
	stateArray[OS_MAIN] = new Main();
	stateArray[OS_POSTLAUNCH] = new PostLaunch();

	// Verify that all states are initialized correctly
	for(uint8_t i = 0; i < OS_NONE; i++) {
		SOAR_ASSERT( stateArray[i] != nullptr );
		SOAR_ASSERT( stateArray[i]->GetStateID() == i );
	}

	os_currentState = stateArray[startingState];

	if (enterStartingState) {
		os_currentState->OnEnter();
	}

	SOAR_PRINT("OSIRIS State Machine Started in [ %s ] state\n", BaseOsirisState::StateToString(os_currentState->GetStateID()));
}

/**
 * @brief Handles state transitions
 * @param nextState The next state to transition to
 * @return The state after the transition
 */
OsirisState OsirisSM::TransitionState(OsirisState nextState)
{
	if( nextState == os_currentState->GetStateID()) {
		return os_currentState->GetStateID();
	}
}

/**
 * @brief Handles current command
 * @param cm The command to handle
 */
void OsirisSM::HandleCommand(Command& cm)
{
    SOAR_ASSERT(os_currentState != nullptr, "Command received before state machine initialized");

    // Handle the command based on the current state
    OsirisState nextOsirisState = os_currentState->HandleCommand(cm);

    // Run transition state - if the next state is the current state this does nothing
    if (nextOsirisState != os_currentState->GetStateID())
    {

        TransitionState(nextOsirisState);
    }
}


/**
 * @brief Returns a string for the state
 */
const char* BaseOsirisState::StateToString(OsirisState stateId)
{
    switch(stateId) {

		default:
			return "WARNING: Invalid";
    }
}
