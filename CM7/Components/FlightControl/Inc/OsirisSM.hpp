/**
 ******************************************************************************
 * File Name          : OsirisSM.hpp
 * Description        : Rocket state machine, handles all rocket state transitions.
 ******************************************************************************
*/
#ifndef SOAR_PAYLOAD_OSIRIS_SM
#define SOAR_PAYLOAD_OSIRIS_SM

#include "Command.hpp"

enum RocketState
{
	RS_ABORT = 0,
	RS_NONE

};

/**
 * @brief External Rocket Control Commands, all fall under GLOBAL_COMMAND -> CONTROL_ACTION umbrella
 *
 *        State specific commands, must be all in-order to avoid duplicate command IDs
 */
enum RocketControlCommands
{
    RSC_FIRST_INVALID = 0,



    //-- TECHNICAL --
    RSC_NONE   // Invalid command, must be last
};

/**
 * @brief Base class for Rocket State Machine
 */
class BaseRocketState
{
public:
    virtual RocketState HandleCommand(Command& cm) = 0; //Handle a command based on the current state
    virtual RocketState OnEnter() = 0;  //Returns the state we're entering
    virtual RocketState OnExit() = 0;   //Returns the state we're exiting

    virtual RocketState GetStateID() { return rsStateID; }

    static const char* StateToString(RocketState stateId);

    //RocketState HandleGeneralStateCommands(RocketControlCommands rcAction);
protected:
    RocketState rsStateID = RS_NONE;    //The name of the state we're in

};

/**
 * @brief Rocket State Machine
 */
class OsirisSM
{
public:
    OsirisSM(RocketState startingState, bool enterStartingState);

    void HandleCommand(Command& cm);

protected:
    RocketState TransitionState(RocketState nextState);

    // Variables
    BaseRocketState* stateArray[RS_NONE];
    BaseRocketState* rs_currentState;
};



#endif // SOAR_AVIONICS_ROCKET_SM
