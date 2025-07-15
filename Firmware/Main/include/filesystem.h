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

#endif /* INCLUDE_FILESYSTEM_H_ */
