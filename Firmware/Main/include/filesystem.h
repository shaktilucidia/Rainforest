/*
 * filesystem.h
 *
 *  Created on: Jul 15, 2025
 *      Author: shakti
 */

#ifndef INCLUDE_FILESYSTEM_H_
#define INCLUDE_FILESYSTEM_H_

#include "../libs/fatfs/ff.h"
#include <stdbool.h>

/**
 * Reading files by this blocks
 */
#define FS_LOAD_BLOCK_SIZE 512

extern FATFS* SDCardFsPtr;

/**
 * Attempt to mount SD-card filesystem
 * @return True if mount successful, false otherwise
 */
bool FS_MountSDCard(void);

/**
 * Unmount previously-mounted SD-card filesystem. Do nothing in case if not mounted
 */
void FS_UnmountSDCard(void);

/**
 * Load file content from path to external memory, starting from startAddress
 */
uint32_t FS_LoadFileToExternalRam
(
	char* path,
	uint32_t startAddr,
	void* memoryDriverContext,
	void (*memoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
);

#endif /* INCLUDE_FILESYSTEM_H_ */
