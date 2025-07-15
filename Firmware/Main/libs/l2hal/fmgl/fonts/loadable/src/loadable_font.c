/*
 * loadable_font.c
 *
 *  Created on: Jul 15, 2025
 *      Author: shakti
 */

#include "../include/loadable_font.h"
#include "../include/loadable_font_private.h"
#include "../../../../include/l2hal_errors.h"
#include "../../../../../fatfs/ff.h"
#include <string.h>
#include <stdlib.h>

void FMGL_LoadableFont_Init
(
	FMGL_LoadableFont_ContextStruct* context,

	char* path,

	void* memoryDriverContext,

	void (*memoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),

	void (*memoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),

	uint32_t baseAddress
)
{
	context->MemoryDriverContext = memoryDriverContext;
	context->MemoryWriteFunctionPtr = memoryWriteFunctionPtr;
	context->MemoryReadFunctionPtr = memoryReadFunctionPtr;
	context->BaseAddress = baseAddress;

	FIL file;
	size_t bytesRead;
	FRESULT fResult = f_open(&file, path, FA_READ);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}


	/* Reading header */
	FMGL_LoadableFont_FileHeaderStruct header;

	fResult = f_read(&file, &header, sizeof(header), &bytesRead);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	if (sizeof(header) != bytesRead)
	{
		L2HAL_Error(Generic);
	}

	/* Signature check */
	if (0 != memcmp(header.Signature, "FMGLFONT", 8))
	{
		L2HAL_Error(Generic);
	}

	/* Version check */
	if (1 != header.Version)
	{
		L2HAL_Error(Generic);
	}

	context->CharactersCount = header.CharactersCount;

	if (context->CharactersCount > 255)
	{
		/* Multibyte encodings aren't supported yet */
		L2HAL_Error(Generic);
	}


	/* Reading characters table items */
	FMGL_LoadableFont_FileCharacterTableItemStruct characterTableItem;
	for (uint32_t i = 0; i < context->CharactersCount; i++)
	{
		fResult = f_read(&file, &characterTableItem, sizeof(characterTableItem), &bytesRead);
		if (fResult != FR_OK)
		{
			L2HAL_Error(Generic);
		}

		if (sizeof(characterTableItem) != bytesRead)
		{
			L2HAL_Error(Generic);
		}

		if (characterTableItem.Code > 255)
		{
			/* Multibyte encodings aren't supported yet */
			L2HAL_Error(Generic);
		}

		characterTableItem.Offset -= 16; /* We have no header in memory */
		context->MemoryWriteFunctionPtr(context->MemoryDriverContext, context->BaseAddress + sizeof(characterTableItem) * i, sizeof(characterTableItem), (uint8_t*)&characterTableItem);
	}

	/* Reading characters data */
	uint32_t nextCharacterDataAddress = context->BaseAddress + 8 * context->CharactersCount;

	FMGL_LoadableFont_FileCharacterDataStruct characterData;
	for (uint32_t i = 0; i < context->CharactersCount; i++)
	{
		/* Width, height, raster size */
		fResult = f_read(&file, &characterData, sizeof(characterData) - sizeof(characterData.Raster), &bytesRead);
		if (fResult != FR_OK)
		{
			L2HAL_Error(Generic);
		}

		if (sizeof(characterData) - sizeof(characterData.Raster) != bytesRead)
		{
			L2HAL_Error(Generic);
		}

		/* Raster */
		characterData.Raster = malloc(characterData.RasterSize);

		fResult = f_read(&file, characterData.Raster, characterData.RasterSize, &bytesRead);
		if (fResult != FR_OK)
		{
			L2HAL_Error(Generic);
		}

		if (characterData.RasterSize != bytesRead)
		{
			L2HAL_Error(Generic);
		}

		context->MemoryWriteFunctionPtr(context->MemoryDriverContext, nextCharacterDataAddress, characterData.RasterSize, characterData.Raster);

		free(characterData.Raster);

		nextCharacterDataAddress += characterData.RasterSize;
	}

	/* Done */
	fResult = f_close(&file);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}
}
