/**
 ******************************************************************************
 * File Name          : OsirisSM.cpp
 * Description        : Primary osiris state machine
 ******************************************************************************
*/
#include <OsirisSM.hpp>
#include "SystemDefines.hpp"
#include "GPIO.hpp"
#include "Data.hpp"
#include "IMUTask//Inc//IMUTask.hpp"

/* Osiris State Machine ------------------------------------------------------------------*/

/**
 * @brief Default constructor for Osiris SM, initializes all states
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
	// Check if we are already in the next state
	if( nextState == os_currentState->GetStateID()) {
		return os_currentState->GetStateID();
	}

	// Return if the next state is invalid
	if(nextState >= OS_NONE) {
		return os_currentState->GetStateID();
	}

	OsirisState previousState = os_currentState->GetStateID();

	// Exit the current state
	os_currentState->OnExit();

	// Set the next state
	os_currentState = stateArray[nextState];

	// Assert that the next state is initialized
	SOAR_ASSERT(os_currentState != nullptr, "os_currentState is nullptr in TransitionState");

	// Enter the current state
	os_currentState->OnEnter();

	SOAR_PRINT("OSIRIS STATE TRANSITION [ %s ] --> [ %s ]\n", BaseOsirisState::StateToString(previousState), BaseOsirisState::StateToString(os_currentState->GetStateID()));

	return os_currentState->GetStateID();
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

/* PreLaunch State ------------------------------------------------------------------*/
/**
 * @brief PreLaunch state constructor
 */
PreLaunch::PreLaunch()
{
    osStateID = OS_PRELAUNCH;
}

/**
 * @brief Entry to PreLaunch state
 * @return The state we're entering
 */
OsirisState PreLaunch::OnEnter()
{
	PreLaunch::CloseAllPeripherals();
	GPIO::LED_GREEN::On();
    return osStateID;
}

/**
 * @brief Exit from PreLaunch state
 * @return The state we're exiting
 */
OsirisState PreLaunch::OnExit()
{
    return osStateID;
}

/**
 * @brief Handle control commands for states : PRELAUNCH, DROGUE
 */
OsirisState PreLaunch::HandleControlSOLS12(OsirisControlCommands ocAction, OsirisState currentState) {
	switch(ocAction) {
		case OSC_OPEN_SOL1:
			GPIO::SOL1::On();
			break;

		case OSC_CLOSE_SOL1:
			GPIO::SOL1::Off();
			break;

		case OSC_OPEN_SOL2:
			GPIO::SOL2::On();
			break;

		case OSC_CLOSE_SOL2:
			GPIO::SOL2::Off();
			break;

		default:
			break;
	}
	return currentState;
}

/**
 * @brief Handle control commands for states : PRELAUNCH, DROGUE, MAIN
 */
OsirisState PreLaunch::HandleControlSOL3Compressor(OsirisControlCommands ocAction, OsirisState currentState) {
	switch(ocAction) {
		case OSC_OPEN_SOL3:
			GPIO::SOL3::On();
			break;

		case OSC_CLOSE_SOL3:
			GPIO::SOL3::Off();
			break;

		case OSC_COMPRESSOR_ON:
			GPIO::COMPRESSOR::On();
			break;

		case OSC_COMPRESSOR_OFF:
			GPIO::COMPRESSOR::Off();
			break;

		default:
			break;
		}
		return currentState;
}

/**
 * @brief Close all peripherals for safety or functionality
 */
void PreLaunch::CloseAllPeripherals() {
	GPIO::SOL1::Off();
	GPIO::SOL2::Off();
	GPIO::SOL3::Off();
	GPIO::COMPRESSOR::Off();
}


/**
 * @brief HandleCommand for PreLaunch state
 * @return The osiris state to transition or stay in
 */
OsirisState PreLaunch::HandleCommand(Command& cm)
{
    OsirisState nextStateID = GetStateID();

    // Switch for the given command
    switch(cm.GetCommand()) {
		case CONTROL_ACTION: {
			switch (cm.GetTaskCommand()) {
				case OSC_PRELAUNCH_TO_LAUNCH:
					// Transition to Launch state
					nextStateID = OS_LAUNCH;
					break;
				case OSC_ANY_TO_PRELAUNCH:
					// Transition to PreLaunch state
					nextStateID = OS_PRELAUNCH;
					break;
				default:
					// Handle as a general control action
					nextStateID = PreLaunch::HandleControlSOLS12((OsirisControlCommands)cm.GetTaskCommand(), GetStateID());
					nextStateID = PreLaunch::HandleControlSOL3Compressor((OsirisControlCommands)cm.GetTaskCommand(), GetStateID());
					break;
			}
			break;
		}
		case DATA_COMMAND: //TODO Delete me, I'm just for a test unpacking this struct
			switch(cm.GetTaskCommand()) {
			case OSC_RECEIVE_LINACC:
				float printBuf[3] = {0, 0, 0};
				char printBuf2[31];
				IMUData* data = (IMUData*)cm.GetDataPointer();
				printBuf[0] = data->xAccel;
				printBuf[1] = data->yAccel;
				printBuf[2] = data->zAccel;
				sprintf(printBuf2, " X: %d\n Y: %d\n Z: %d\n", printBuf[0], printBuf[1], printBuf[2]);
				SOAR_PRINT(printBuf2);
			}
		default:
			// Do nothing
			break;
    }

    // Make sure to reset the command, and return the next state
    cm.Reset();
    return nextStateID;
}

/* Launch State ------------------------------------------------------------------*/
/**
 * @brief Launch state constructor
 */
Launch::Launch()
{
    osStateID = OS_LAUNCH;
}

/**
 * @brief Entry to Launch state
 * @return The state we're entering
 */
OsirisState Launch::OnEnter()
{
	PreLaunch::CloseAllPeripherals();
    return osStateID;
}

/**
 * @brief Exit from Launch state
 * @return The state we're exiting
 */
OsirisState Launch::OnExit()
{
    return osStateID;
}

/**
 * @brief HandleCommand for Launch state
 * @return The osiris state to transition or stay in
 */
OsirisState Launch::HandleCommand(Command& cm)
{
    OsirisState nextStateID = GetStateID();

    // Switch for the given command
    switch(cm.GetCommand()) {
		case CONTROL_ACTION: {
			switch (cm.GetTaskCommand()) {
				case OSC_LAUNCH_TO_DROGUE:
					// Transition to Drogue state
					nextStateID = OS_DROGUE;
					break;
				case OSC_ANY_TO_PRELAUNCH:
					// Transition to PreLaunch state
					nextStateID = OS_PRELAUNCH;
					break;
				default:
					break;
			}
			break;
		}
		default:
			// Do nothing
			break;
    }

    // Make sure to reset the command, and return the next state
    cm.Reset();
    return nextStateID;
}

/* Drogue State ------------------------------------------------------------------*/
/**
 * @brief Drogue state constructor
 */
Drogue::Drogue()
{
    osStateID = OS_DROGUE;
}

/**
 * @brief Entry to Drogue state
 * @return The state we're entering
 */
OsirisState Drogue::OnEnter()
{
    return osStateID;
}

/**
 * @brief Exit from Drogue state
 * @return The state we're exiting
 */
OsirisState Drogue::OnExit()
{
    return osStateID;
}

/**
 * @brief HandleCommand for Drogue state
 * @return The osiris state to transition or stay in
 */
OsirisState Drogue::HandleCommand(Command& cm)
{
    OsirisState nextStateID = GetStateID();

    // Switch for the given command
    switch(cm.GetCommand()) {
		case CONTROL_ACTION: {
			switch (cm.GetTaskCommand()) {
				case OSC_DROGUE_TO_MAIN:
					// Transition to Main state
					nextStateID = OS_MAIN;
					break;
				case OSC_ANY_TO_PRELAUNCH:
					// Transition to PreLaunch state
					nextStateID = OS_PRELAUNCH;
					break;
				default:
					// Handle as a general control action
					nextStateID = PreLaunch::HandleControlSOLS12((OsirisControlCommands)cm.GetTaskCommand(), GetStateID());
					nextStateID = PreLaunch::HandleControlSOL3Compressor((OsirisControlCommands)cm.GetTaskCommand(), GetStateID());
					break;
			}
			break;
		}
		default:
			// Do nothing
			break;
    }

    // Make sure to reset the command, and return the next state
    cm.Reset();
    return nextStateID;
}

/* Main State ------------------------------------------------------------------*/
/**
 * @brief Main state constructor
 */
Main::Main()
{
    osStateID = OS_MAIN;
}

/**
 * @brief Entry to Main state
 * @return The state we're entering
 */
OsirisState Main::OnEnter()
{
	PreLaunch::CloseAllPeripherals();
    return osStateID;
}

/**
 * @brief Exit from Main state
 * @return The state we're exiting
 */
OsirisState Main::OnExit()
{
    return osStateID;
}

/**
 * @brief HandleCommand for Main state
 * @return The osiris state to transition or stay in
 */
OsirisState Main::HandleCommand(Command& cm)
{
    OsirisState nextStateID = GetStateID();

    // Switch for the given command
    switch(cm.GetCommand()) {
		case CONTROL_ACTION: {
			switch (cm.GetTaskCommand()) {
				case OSC_MAIN_TO_POSTLAUNCH:
					// Transition to PostLaunch state
					nextStateID = OS_POSTLAUNCH;
					break;
				case OSC_ANY_TO_PRELAUNCH:
					// Transition to PreLaunch state
					nextStateID = OS_PRELAUNCH;
					break;
				default:
					// Handle as a general control action
					nextStateID = PreLaunch::HandleControlSOL3Compressor((OsirisControlCommands)cm.GetTaskCommand(), GetStateID());
					break;
			}
			break;
		}
		default:
			// Do nothing
			break;
    }

    // Make sure to reset the command, and return the next state
    cm.Reset();
    return nextStateID;
}

/* PostLaunch State ------------------------------------------------------------------*/
/**
 * @brief PostLaunch state constructor
 */
PostLaunch::PostLaunch()
{
    osStateID = OS_POSTLAUNCH;
}

/**
 * @brief Entry to PostLaunch state
 * @return The state we're entering
 */
OsirisState PostLaunch::OnEnter()
{
	PreLaunch::CloseAllPeripherals();
    return osStateID;
}

/**
 * @brief Exit from PostLaunch state
 * @return The state we're exiting
 */
OsirisState PostLaunch::OnExit()
{
    return osStateID;
}

/**
 * @brief HandleCommand for PostLaunch state
 * @return The osiris state to transition or stay in
 */
OsirisState PostLaunch::HandleCommand(Command& cm)
{
    OsirisState nextStateID = GetStateID();

    // Switch for the given command
    switch(cm.GetCommand()) {
		case CONTROL_ACTION: {
			switch (cm.GetTaskCommand()) {
				case OSC_ANY_TO_PRELAUNCH:
					// Transition to PreLaunch state
					nextStateID = OS_PRELAUNCH;
					break;

				default:
					break;
			}
			break;
		}
		default:
			// Do nothing
			break;
    }

    // Make sure to reset the command, and return the next state
    cm.Reset();
    return nextStateID;
}

/**
 * @brief Returns a string for the state
 */
const char* BaseOsirisState::StateToString(OsirisState stateId)
{
    switch(stateId) {
    	case OS_PRELAUNCH:
    		return "PreLaunch";
    		break;

    	case OS_LAUNCH:
    		return "Launch";
    		break;

    	case OS_DROGUE:
			return "Drogue";
    		break;

    	case OS_MAIN:
			return "Main";
    		break;

    	case OS_POSTLAUNCH:
    		return "PostLaunch";
    		break;

    	case OS_NONE:
    		return "None";
    		break;

		default:
			return "WARNING: Invalid";
    }
}
