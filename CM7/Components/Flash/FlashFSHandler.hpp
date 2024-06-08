/*
 * FlashFSHandler.cpp
 *
 *  Created on: Jun 8, 2024
 *      Author: goada
 */


#include "LittleFS/lfs.h"
#include "../Drivers/W25N01_Drivers/W25N01GVSFIG.hpp"



class FlashFileSystem {
public:

	bool Init();
	inline bool GetInitialized();

	void TestVerify();
	bool OpenFile(const char* const filename);

private:
	bool initialized;


	void CloseFile(lfs_file_t *file);

	lfs_t LFS;
	lfs_file_t currentOpenFile;
	bool currentOpenFileValid;
	W25N01GVSFIG flashChipHandle;

    int (*readWrapper)(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size);

    int (*writeWrapper)(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, const void *buffer, lfs_size_t size);

    int (*eraseWrapper)(const struct lfs_config *c, lfs_block_t block);

	const lfs_config LFSCONFIG = {
		.read = readWrapper,
		.prog = writeWrapper,
		.erase = eraseWrapper,

		.read_size = 16,
		.prog_size = 16,
		.block_size = 0x20000,
		.block_count = 0x400,
		.block_cycles = 500,
		.cache_size = 128,
		.lookahead_size = 0x800,



	};
};
