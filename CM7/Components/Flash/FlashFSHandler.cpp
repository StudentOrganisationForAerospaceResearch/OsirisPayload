/*
 * FlashFSHandler.cpp
 *
 *  Created on: Jun 8, 2024
 *      Author: goada
 */

#include "FlashFSHandler.hpp"
#include "Command.hpp"
#include "CubeDefines.hpp"

bool FlashFileSystem::Init() {
	SOAR_PRINT("Gloobing...\n");

	int mountattempts = 0;

	while(1) {
		int e = lfs_mount(&LFS, &LFSCONFIG);

		if(mountattempts == 0) {
			if(e == 0) {
				// Successfully mounted
				SOAR_PRINT("Mounted LittleFS.\n");
				return (initialized = true);
			} else {
				SOAR_PRINT("Could not mount LittleFS,  reformatting...\n");
				mountattempts++;
				lfs_format(&LFS, &LFSCONFIG);
				HAL_Delay(500);
			}
		} else {
			if(e == 0) {
				// Successfully mounted
				SOAR_PRINT("Mounted LittleFS.\n");
				return (initialized = true);
			} else {
				SOAR_PRINT("Could still not mount LittleFS after reformat.\n");
				return false;
			}
		}
	}

}

inline bool FlashFileSystem::GetInitialized() {
	return initialized;
}

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
