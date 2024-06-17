/*
 * FlashFSHandler.cpp
 *
 *  Created on: Jun 8, 2024
 *      Author: goada
 */

#include "Command.hpp"
#include "Task.hpp"
#include "FlashTask//Inc//FlashTask.hpp"
#include "Data.hpp"

FlashTask::FlashTask() : Task(FLASH_TASK_QUEUE_DEPTH_OBJS)
{
}

void FlashTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize flash task twice");

    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)FlashTask::RunTask,
            (const char*)"FlashTask",
            (uint16_t)FLASH_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)FLASH_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "FlashTask::InitTask() - xTaskCreate() failed");
}


void FlashTask::Run()
{

	while(flashChipHandle.init() != FLASH_OK) {osDelay(30);}
	uint8_t buf = 0xff;
	stateLoggingAddress = 0;
	do
	{
		flashChipHandle.read(&buf, stateLoggingAddress, 1);
		stateLoggingAddress += 20;
	} while (buf == 0);

	address.u32 = stateLoggingAddress;
	stateLoggingAddress -= 20;
	buf = 0;
	flashChipHandle.write(&buf, stateLoggingAddress, 1);

	while (1) {

	        //Process commands in blocking mode
	        Command cm;
	        bool res = qEvtQueue->ReceiveWait(cm);
	        if(res)
	            handleCommand(cm);

	    }
}


void FlashTask::handleCommand(Command& com)
{
	switch(com.GetCommand())
	{
	case TASK_SPECIFIC_COMMAND: {
		switch (com.GetTaskCommand())
		{
			case FHT_WRITE_DATA: {
				/* Expects an logFloat* array where:
				 *  - Element 1 is a pointer to an AllData set (cast as uint16_t*)
				 *  - 2 is a pointer to the altitude to be recorded
				 */

				logFloat* toWrite[13] =
				{
					&(((AllData*)com.GetDataPointer())->time),
					&(((AllData*)com.GetDataPointer())->imuData->xAccel),
					&(((AllData*)com.GetDataPointer())->imuData->yAccel),
					&(((AllData*)com.GetDataPointer())->imuData->zAccel),
					&(((AllData*)com.GetDataPointer())->altimeterData->altitude),
					&(((AllData*)com.GetDataPointer())->altimeterData->temp),
					&(((AllData*)com.GetDataPointer())->barometerData->marioPressure),
					&(((AllData*)com.GetDataPointer())->barometerData->marioTemperature),
					&(((AllData*)com.GetDataPointer())->barometerData->luigiPressure),
					&(((AllData*)com.GetDataPointer())->barometerData->luigiTemperature),
					&(((AllData*)com.GetDataPointer())->barometerData->bowserPressure),
					&(((AllData*)com.GetDataPointer())->barometerData->bowserTemperature),
					(logFloat*)(com.GetDataPointer() + 1)
				};

				for (uint8_t i = 0; i < 13; i++)
				{
					if (address.u32 <= 100000000)
					{
						flashChipHandle.write(toWrite[i]->u8, address.u32, 4);
						address.u32 += 4;
					} else {
						return;
					}

				}
				break;
			}
			case FHT_APOGEE: {
				flashChipHandle.write(com.GetDataPointer(), stateLoggingAddress + 5, 4);
				break;
			}
			case FHT_MAIN_1: {
				flashChipHandle.write(com.GetDataPointer(), stateLoggingAddress + 7, 4);
				break;
			}
			case FHT_MAIN_2: {
				flashChipHandle.write(com.GetDataPointer(), stateLoggingAddress + 13, 4);
				break;
			}
			case FHT_GROUND: {
				flashChipHandle.write(com.GetDataPointer(), stateLoggingAddress + 17, 4);
				flashChipHandle.write(address.u8, stateLoggingAddress + 1, 4);
				break;
			}
			case FHT_ERASE: {
				flashChipHandle.eraseChip();
				SOAR_PRINT("done\n");
				break;
			}
		}
	}
	default:
		break;
	}
}


/*
bool FlashFileSystem::Init() {
	int mountattempts = 0;
	e_flash_status f = flashChipHandle.init();
	if(f == FLASH_OK) {
		SOAR_PRINT("Initialized flash chip\n");
	} else {
		SOAR_PRINT("Could not initialize flash chip.\n");
		return false;
	}

#if 0
	flashChipHandle.eraseChip();
	uint8_t* test_data = new uint8_t[5000];
	for(int i = 0; i < 5000; i++) {
		test_data[i] = i % 256;
	}
	flashChipHandle.write(test_data, 0x00, 5000);
	memset(test_data,0x09,5000);
	flashChipHandle.read(test_data, 0x00, 5000);
	for(auto i = 0; i < 5000; i++) {
		if(test_data[i] != i%256) {
			SOAR_PRINT("Read fail at %d, wanted %d, was %d\n",i, i%256, test_data[i]);
		}
	}
	SOAR_PRINT("Read complete 1\n");


	for(int i = 0; i < 5000; i++) {
		test_data[i] = (i*i) % 256;
	}
	flashChipHandle.write(test_data, 2001, 5000);
	memset(test_data,0x09,5000);
	flashChipHandle.read(test_data, 2001, 5000);
	for(auto i = 0; i < 5000; i++) {
		if(test_data[i] != (i*i)%256) {
			SOAR_PRINT("Read fail at %d, wanted %d, was %d\n",i, (i*i)%256, test_data[i]);
		}
	}

	SOAR_PRINT("Read complete 2\n");
	//flashChipHandle.eraseSmallestSection(0x62);
	//flashChipHandle.write(test_data, 0x62, sizeof(test_data));
	//memset(test_data,0x00,sizeof(test_data));
	//flashChipHandle.read(test_data, 0x62, sizeof(test_data));
#endif

	return true;
}*/

/*
void FlashFileSystem::TestVerify() {

	OpenFile("test.hi");
	char testbuf[123];
	for(size_t i = 0; i < sizeof(testbuf); i++) {
		testbuf[i] = i%('~'-'!') + '!';
	}
	testbuf[sizeof(testbuf)-1] = 0x00;

	lfs_file_write(&LFS, &currentOpenFile, testbuf, sizeof(testbuf));
	lfs_file_sync(&LFS, &currentOpenFile);

	printf("Written: %s\n",testbuf);

	memset(testbuf,0x00,sizeof(testbuf));

	lfs_file_read(&LFS, &currentOpenFile, testbuf, sizeof(testbuf));

	printf("Recovered: %s\n",testbuf);

	CloseFile(&currentOpenFile);
}
/*
bool FlashFileSystem::OpenFile(const char *const filename) {
	if(currentOpenFileValid) {
		CloseFile(&currentOpenFile);
	}

	int e = lfs_file_open(&LFS, &currentOpenFile, filename, LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC);
	if(e == 0) {
		currentOpenFileValid = true;
		return true;
	} else {
		return false;
	}
}

void FlashFileSystem::CloseFile(lfs_file_t *file) {
	lfs_file_close(&LFS, file);
	currentOpenFileValid = false;
}

int lfs_ReadWrapper (const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, void *buffer, lfs_size_t size) {
	SOAR_PRINT("readwrapper\n");

	e_flash_status s = ((FlashFileSystem*)(c->context))->flashChipHandle.read((uint8_t*)buffer, block*c->block_size + off, size);

	while(((FlashFileSystem*)(c->context))->flashChipHandle.isBusy());
	return s;
}


int lfs_WriteWrapper (const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, const void *buffer, lfs_size_t size) {
	SOAR_PRINT("writewrapper\n");
	e_flash_status s = ((FlashFileSystem*)(c->context))->flashChipHandle.write((uint8_t*)buffer, block*c->block_size + off, size);

	while(((FlashFileSystem*)(c->context))->flashChipHandle.isBusy());
	return s;
}

int lfs_SyncWrapper (const struct lfs_config *c) {
	SOAR_PRINT("sync wrapper\n");
	return 0;
}

int lfs_EraseWrapper (const struct lfs_config *c, lfs_block_t block) {
	SOAR_PRINT("erasewrapper\n");
	e_flash_status s = ((FlashFileSystem*)(c->context))->flashChipHandle.eraseSmallestSection(block*c->block_size);

	while(((FlashFileSystem*)(c->context))->flashChipHandle.isBusy());
	return s;
}

bool FlashFileSystem::WriteAt(uint8_t *data, size_t size, uint32_t addr) {
	return flashChipHandle.write(data, addr, size) == FLASH_OK;
}

bool FlashFileSystem::ReadAt(uint8_t *dataout, size_t size, uint32_t addr) {
	return flashChipHandle.read(dataout, addr, size) == FLASH_OK;
}

bool FlashFileSystem::EraseBlock(uint32_t addr) {
	return flashChipHandle.eraseSmallestSection(addr) == FLASH_OK;
}

bool FlashFileSystem::EraseChip() {
	return flashChipHandle.eraseChip() == FLASH_OK;
}
*/

