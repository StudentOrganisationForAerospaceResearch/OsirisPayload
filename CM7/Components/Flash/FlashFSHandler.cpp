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


	// Use LittleFS (currently does not work)
#if 0
	while(1) {
		int e = lfs_mount(&LFS, &LFSCONFIG);

		if(mountattempts == 0) {
			if(e == 0) {
				// Successfully mounted
				SOAR_PRINT("Mounted LittleFS.\n");
				return (initialized = true);
			} else {

				SOAR_PRINT("Could not mount LittleFS (%d),  reformatting...\n", e);
				mountattempts++;
				int r = lfs_format(&LFS, &LFSCONFIG);
				if(r != 0) {
					SOAR_PRINT("Failed format %d\n",r);
					return false;
				}
				SOAR_PRINT("Formatted\n");
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
#endif

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

//bool FlashFileSystem::WriteAtFilePos(const uint8_t* buf, size_t size, uint32_t pos) {

//}
