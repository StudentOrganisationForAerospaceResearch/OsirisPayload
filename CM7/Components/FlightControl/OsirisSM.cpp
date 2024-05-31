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
OsirisSM::OsirisSM(RocketState startingState, bool enterStartingState)
{

}

/**
 * @brief Handles state transitions
 * @param nextState The next state to transition to
 * @return The state after the transition
 */
RocketState OsirisSM::TransitionState(RocketState nextState)
{

}

/**
 * @brief Handles current command
 * @param cm The command to handle
 */
void OsirisSM::HandleCommand(Command& cm)
{
    SOAR_ASSERT(rs_currentState != nullptr, "Command received before state machine initialized");

    // Handle the command based on the current state
    RocketState nextRocketState = rs_currentState->HandleCommand(cm);

    // Run transition state - if the next state is the current state this does nothing
    if (nextRocketState != rs_currentState->GetStateID())
    {

        TransitionState(nextRocketState);
    }
}

/* Base State ------------------------------------------------------------------*/
///**
// * @brief General handler for actions that should be supported by all rocket state machines
// */
//RocketState BaseRocketState::HandleGeneralStateCommands(RocketControlCommands rcAction)
//{
//    switch (rcAction) {
//    case RSC_PAUSE_LOGGING:
//        //TODO: Send pause logging command
//        break;
//    case RSC_START_LOGGING:
//        //TODO: Send start logging command
//        break;
//    default:
//        break;
//    }
//
//    return GetStateID();
//}




/**
 * @brief Returns a string for the state
 */
const char* BaseRocketState::StateToString(RocketState stateId)
{
    switch(stateId) {

		default:
			return "WARNING: Invalid";
    }
}
