/*
 * FlashFSHandler.cpp
 *
 *  Created on: Jun 8, 2024
 *      Author: goada
 */


/*Notes on the organization of things:
 *
 *  - First page is reserved for the following:
 *  	- 8 0's to flag where data is (and prevent it from being overwritten)
 *  	- 32 bit address of last raw date write
 *  	- 32 bit time of apoggee detection (transition from launch to drogue states)
 *  	- 2x 32 bit time of main detection (transition from drouge to main states)
 *  	- 32 bit time of ground detection (transition from main to postlaunch)
 *
 *  - Every page is written as 5 writes of 400 bits
 *  	- 1 copy of an AllData struct (descibed in data.hpp)
 *  	- unsigned 16 bit filtered altitude
 *
 */

#include "Sensors//Inc//W25N01GVSFIG.hpp"
#include "Task.h"
#include "SystemDefines.hpp"
#include "Data.hpp"

enum firstPageDataType {
	lastWriteAddress,
	apogeeTime,
	mainDeactivateTime,
	mainAcitvateTime,
	groundTime
};

enum flashTaskCommands {
	FHT_WRITE_DATA,
	FHT_APOGEE,
	FHT_MAIN_1,
	FHT_MAIN_2,
	FHT_GROUND,
	FHT_ERASE
};


class FlashTask: public Task {
public:

	static FlashTask& Inst() {
		static FlashTask inst;
		return inst;
	}

	void InitTask();

	bool logTransition(firstPageDataType);

	bool write(struct AllData* data, uint16_t altitude);

protected:

	static void RunTask(void* Pvparams) {FlashTask::Inst().Run();}
	void Run();

	void handleCommand(Command& com);

private:

	FlashTask();        // Private constructor
	FlashTask(const FlashTask&);                        // Prevent copy-construction
	FlashTask& operator=(const FlashTask&);            // Prevent assignment

	void TestVerify();

	W25N01GVSFIG flashChipHandle = {GPIOB, GPIO_PIN_10, nullptr, 0, 0};

	logFloat address;
	uint8_t stateLoggingAddress;

	/*
	bool WriteAt(uint8_t* data, size_t size, uint32_t addr);
	bool ReadAt(uint8_t* dataout, size_t size, uint32_t addr);
	bool EraseBlock(uint32_t addr);
	bool EraseChip();


	bool initialized;

	lfs_t LFS;
	lfs_file_t currentOpenFile;
	bool currentOpenFileValid;


	const lfs_config LFSCONFIG = {
		.context = this,
		.read = lfs_ReadWrapper,
		.prog = lfs_WriteWrapper,
		.erase = lfs_EraseWrapper,
		.sync = lfs_SyncWrapper,



		.read_size = 16,
		.prog_size = 16,
		.block_size = 0x20000,
		.block_count = 1024,
		.block_cycles = 500,
		.cache_size =16,
		.lookahead_size = 16,

		//.read_buffer = readbuf,
		//.prog_buffer = progbuf,
		//.lookahead_buffer = lookbuf






	};
	*/
};
