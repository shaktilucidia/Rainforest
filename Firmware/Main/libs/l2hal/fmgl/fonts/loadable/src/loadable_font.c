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
#include <stdbool.h>

FMGL_API_Font FMGL_LoadableFont_Init
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


	/* Reading characters table items to get offsets */
	memset(context->CharacterDataAddresses, 0, FMGL_LOADABLE_FONT_MAX_CHARACTERS_COUNT);

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

		/* We have no header and characters table in external memory, so -16 - 8 * CharactersCount */
		context->CharacterDataAddresses[characterTableItem.Code] = context->BaseAddress + characterTableItem.Offset - 16 - 8 * context->CharactersCount;
	}

	/* Filling nonexistent characters */
	for (uint32_t i = 0; i < FMGL_LOADABLE_FONT_MAX_CHARACTERS_COUNT; i++)
	{
		if (0 == context->CharacterDataAddresses[i])
		{
			context->CharacterDataAddresses[i] = context->CharacterDataAddresses[0]; /* Character with code 0 is nonexistent character */
		}
	}

	/* Reading characters data */
	uint32_t nextCharacterDataAddress = context->BaseAddress;

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

		context->MemoryWriteFunctionPtr(context->MemoryDriverContext, nextCharacterDataAddress, sizeof(characterData) - sizeof(characterData.Raster), (uint8_t*)&characterData);

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

		context->MemoryWriteFunctionPtr(context->MemoryDriverContext, nextCharacterDataAddress + sizeof(characterData) - sizeof(characterData.Raster), characterData.RasterSize, characterData.Raster);

		free(characterData.Raster);

		nextCharacterDataAddress += characterData.RasterSize + sizeof(characterData) - sizeof(characterData.Raster);
	}

	/* Done */
	fResult = f_close(&file);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	FMGL_API_Font font;
	font.Height = FMGL_LoadableFont_GetCharacterHeight(context, ' '); /* Use space as height base */
	font.GetCharacterWidth = (uint16_t (*)(void *, uint8_t))&FMGL_LoadableFont_GetCharacterWidth;
	font.GetCharacterRaster = (const uint8_t* (*)(void *, uint8_t))&FMGL_LoadableFont_GetCharacterRaster;
	font.IsLoadable = true;
	font.Context = context;

	return font;
}


FMGL_LoadableFont_FileCharacterDataStruct FMGL_LoadableFont_GetCharacterData(FMGL_LoadableFont_ContextStruct* context, uint8_t character)
{
	FMGL_LoadableFont_FileCharacterDataStruct characterData;
	context->MemoryReadFunctionPtr(context->MemoryDriverContext, context->CharacterDataAddresses[character], sizeof(characterData) - sizeof(characterData.Raster), (uint8_t*)&characterData);

	return characterData;
}

uint16_t FMGL_LoadableFont_GetCharacterWidth(FMGL_LoadableFont_ContextStruct* context, uint8_t character)
{
	return (uint16_t)FMGL_LoadableFont_GetCharacterData(context, character).Width;
}

uint16_t FMGL_LoadableFont_GetCharacterHeight(FMGL_LoadableFont_ContextStruct* context, uint8_t character)
{
	return (uint16_t)FMGL_LoadableFont_GetCharacterData(context, character).Height;
}

uint8_t* FMGL_LoadableFont_GetCharacterRaster(FMGL_LoadableFont_ContextStruct* context, uint8_t character)
{
	uint32_t characterDataAddress = context->CharacterDataAddresses[character];

	FMGL_LoadableFont_FileCharacterDataStruct characterData;
	context->MemoryReadFunctionPtr(context->MemoryDriverContext, characterDataAddress, sizeof(characterData) - sizeof(characterData.Raster), (uint8_t*)&characterData);

	uint8_t* raster = malloc(characterData.RasterSize);

	context->MemoryReadFunctionPtr(context->MemoryDriverContext, characterDataAddress + sizeof(characterData) - sizeof(characterData.Raster), characterData.RasterSize, raster);

	return raster;
}
