/**
 ******************************************************************************
 * File Name          : OsirisSM.hpp
 * Description        : Osiris state machine, handles all osiris state transitions.
 ******************************************************************************
*/
#ifndef SOAR_PAYLOAD_OSIRIS_SM
#define SOAR_PAYLOAD_OSIRIS_SM

#include "Command.hpp"


enum OsirisState
{
	// -- ON GROUND --
	OS_PRELAUNCH = 0, // All operations allowed in this state. Used for testing on ground

	// -- TRANSITION ON LAUNCH DETECTION --
	OS_LAUNCH, // Normal rocket flight, do nothing

	// -- TRANSITION ON DROGUE DEPLOYMENT --
	OS_DROGUE, // Opening SOLS 1 - 3

	// -- TRANSITION ON MAIN DEPLOYMENT (1800FT) --
	OS_MAIN, // SOL 3 Open

	// -- TRANSITION AT 300 FT AGL --
	OS_POSTLAUNCH, // All SOLS closed, do nothing

	// -- INVALID STATE --
	OS_NONE

};

/**
 * @brief External Osiris Control Commands, all fall under GLOBAL_COMMAND -> CONTROL_ACTION umbrella
 *
 *        State specific commands, must be all in-order to avoid duplicate command IDs
 */
enum OsirisControlCommands
{
    OSC_FIRST_INVALID = 0,

	OSC_ANY_TO_PRELAUNCH,

	// -- PRELAUNCH --
	OSC_PRELAUNCH_TO_LAUNCH,
	OSC_PRELAUNCH_TO_DROGUE,	// Used to bypass launch state if launch detection fails

	// -- LAUNCH --
	OSC_LAUNCH_TO_DROGUE,

	// -- DROGUE --
	OSC_DROGUE_TO_MAIN,

	// -- MAIN --
	OSC_MAIN_TO_POSTLAUNCH,

	// -- POSTLAUNCH --
	// No unique commands. Use OSC_ANY_TO_PRELAUNCH to cycle states again

	// -- CONTROL COMMANDS --
	OSC_OPEN_SOL1,
	OSC_CLOSE_SOL1,
	OSC_OPEN_SOL2,
	OSC_CLOSE_SOL2,
	OSC_OPEN_SOL3,
	OSC_CLOSE_SOL3,
	OSC_COMPRESSOR_ON,
	OSC_COMPRESSOR_OFF,

	OSC_RECEIVE_LINACC,
	OSC_RECEIVE_ANGACC,

	OSC_RECEIVE_PRESSURE,
	OSC_RECEIVE_TEMPERATURE,

    // -- TECHNICAL --
    OSC_NONE   // Invalid command, must be last
};

/**
 * @brief Base class for Osiris State Machine
 */
class BaseOsirisState
{
public:
    virtual OsirisState HandleCommand(Command& cm) = 0; //Handle a command based on the current state
    virtual OsirisState OnEnter() = 0;  //Returns the state we're entering
    virtual OsirisState OnExit() = 0;   //Returns the state we're exiting

    virtual OsirisState GetStateID() { return osStateID; }

    static const char* StateToString(OsirisState stateId);

protected:
    OsirisState osStateID = OS_NONE;    //The name of the state we're in

};

/**
 * @brief Osiris State Machine
 */
class OsirisSM
{
public:
    OsirisSM(OsirisState startingState, bool enterStartingState);

    void HandleCommand(Command& cm);

    OsirisState GetStateID() { return os_currentState->GetStateID(); }

protected:
    OsirisState TransitionState(OsirisState nextState);

    // Variables
    BaseOsirisState* stateArray[OS_NONE];
    BaseOsirisState* os_currentState;
};


/**
 * @brief PreLaunch State
 * 		-> Allows full control
 * 		-> Used for testing and standby on the ground
 */
class PreLaunch : public BaseOsirisState
{
	public:
		PreLaunch();

		// Base Class functions
		OsirisState HandleCommand(Command& cm) override;
		OsirisState OnEnter() override;
		OsirisState OnExit() override;

		static OsirisState HandleControlSOLS12(OsirisControlCommands ocAction, OsirisState currentState);
		static OsirisState HandleControlSOL3Compressor(OsirisControlCommands ocAction, OsirisState currentState);
		static void CloseAllPeripherals();

	private:
};

/**
 * @brief Launch State
 * 		-> Normal rocket flight
 * 		-> Do nothing
 */
class Launch : public BaseOsirisState
{
	public:
		Launch();

		// Base Class functions
		OsirisState HandleCommand(Command& cm) override;
		OsirisState OnEnter() override;
		OsirisState OnExit() override;

	private:

};

/**
 * @brief Drogue State
 * 		-> Opening SOLS 1 - 3 at different heights
 */
class Drogue : public BaseOsirisState
{
	public:
		Drogue();

		// Base Class functions
		OsirisState HandleCommand(Command& cm) override;
		OsirisState OnEnter() override;
		OsirisState OnExit() override;

	private:

};

/**
 * @brief Main State
 * 		-> SOL3 Open
 * 		-> SOL3 Open close 150ft before and after main state
 */
class Main : public BaseOsirisState
{
	public:
		Main();

		// Base Class functions
		OsirisState HandleCommand(Command& cm) override;
		OsirisState OnEnter() override;
		OsirisState OnExit() override;

	private:

};

/**
 * @brief PostLaunch State
 * 		-> All SOLS closed
 * 		-> Do nothing
 */
class PostLaunch : public BaseOsirisState
{
	public:
		PostLaunch();

		// Base Class functions
		OsirisState HandleCommand(Command& cm) override;
		OsirisState OnEnter() override;
		OsirisState OnExit() override;

	private:

};


#endif // SOAR_PAYLOAD_OSIRIS_SM
