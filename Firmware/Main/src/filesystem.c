/*
 * filesystem.c
 *
 *  Created on: Jul 15, 2025
 *      Author: shakti
 */

#include "../include/filesystem.h"
#include <stddef.h>
#include <stdlib.h>

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
