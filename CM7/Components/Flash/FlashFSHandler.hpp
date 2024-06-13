/*
 * FlashFSHandler.cpp
 *
 *  Created on: Jun 8, 2024
 *      Author: goada
 */


#include "LittleFS/lfs.h"
#include "../Drivers/W25N01_Drivers/W25N01GVSFIG.hpp"
#include "main.h"

int lfs_ReadWrapper (const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, void *buffer, lfs_size_t size);

int lfs_WriteWrapper(const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, const void *buffer, lfs_size_t size);

int lfs_EraseWrapper(const struct lfs_config *c, lfs_block_t block);

int lfs_SyncWrapper(const struct lfs_config *c);

static uint8_t readbuf[2112];
static uint8_t progbuf[2112];
static uint8_t lookbuf[2112];

class FlashFileSystem {
public:

	bool Init();
	inline bool GetInitialized();

	void TestVerify();

	bool OpenFile(const char* const filename);
	void CloseFile(lfs_file_t *file);

	//bool WriteAtFilePos(const uint8_t* buf, size_t size, uint32_t pos);

	W25N01GVSFIG flashChipHandle = {GPIOB, GPIO_PIN_10, nullptr, 0, 0};

private:
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
};
