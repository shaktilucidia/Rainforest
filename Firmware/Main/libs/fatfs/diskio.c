/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "../l2hal/l2hal_config.h"

extern L2HAL_SDCard_ContextStruct SDCardContext;


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0; /* Disk is ready */
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	return 0; /* Successfully initialized */
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	BYTE* bufferPtr = buff;
	for (uint32_t blockNumber = sector; blockNumber < sector + count; blockNumber++)
	{
		L2HAL_SDCard_ReadSingleBlock(&SDCardContext, blockNumber, bufferPtr);
		bufferPtr += L2HAL_SDCARD_BLOCK_SIZE;
	}

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	BYTE* bufferPtr = buff;
	for (uint32_t blockNumber = sector; blockNumber < sector + count; blockNumber++)
	{
		L2HAL_SDCard_WriteSingleBlock(&SDCardContext, blockNumber, bufferPtr);
		bufferPtr += L2HAL_SDCARD_BLOCK_SIZE;
	}

	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (cmd)
	{
		case CTRL_SYNC:
			/* Do nothing */
			break;

		case GET_SECTOR_COUNT:
			*((LBA_t*)buff) = L2HAL_SDCard_ReadBlocksCount(&SDCardContext);
			break;

		case GET_SECTOR_SIZE:
			*(WORD*)buff = L2HAL_SDCARD_BLOCK_SIZE;
			break;

		case GET_BLOCK_SIZE:
			*(DWORD*)buff = 1;
			break;

		case CTRL_TRIM:
			/* Do nothing */
			break;
	}

	return RES_OK;
}

