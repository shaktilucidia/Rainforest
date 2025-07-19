/*
 * filesystem.c
 *
 *  Created on: Jul 15, 2025
 *      Author: shakti
 */

#include "../include/filesystem.h"
#include <stddef.h>
#include <stdlib.h>
#include "../libs/l2hal/include/l2hal_errors.h"

bool FS_MountSDCard(void)
{
	SDCardFsPtr = malloc(sizeof(FATFS));
	FRESULT fResult = f_mount(SDCardFsPtr, "0", 1);

	if (FR_OK == fResult)
	{
		return true;
	}

	free(SDCardFsPtr);
	SDCardFsPtr = NULL;

	return false;
}


void FS_UnmountSDCard(void)
{
	if (NULL == SDCardFsPtr)
	{
		return;
	}

	f_unmount("0");

	free(SDCardFsPtr);
	SDCardFsPtr = NULL;
}

uint32_t FS_LoadFileToExternalRam
(
	char* path,
	uint32_t startAddr,
	void* memoryDriverContext,
	void (*memoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
)
{
	FIL file;
	FRESULT fResult = f_open(&file, path, FA_READ);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	uint32_t fileSize = f_size(&file);

	uint8_t buffer[FS_LOAD_BLOCK_SIZE];
	uint32_t bytesRead;
	uint32_t totalRead = 0;

	while (totalRead < fileSize)
	{
		fResult = f_read(&file, buffer, FS_LOAD_BLOCK_SIZE, (size_t*)&bytesRead);
		if (fResult != FR_OK)
		{
			L2HAL_Error(Generic);
		}

		memoryWriteFunctionPtr(memoryDriverContext, startAddr + totalRead, bytesRead, buffer);
		totalRead += bytesRead;
	}


	/* Done */
	fResult = f_close(&file);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	return fileSize;
}
