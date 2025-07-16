/*
 * loadable_font_private.h
 *
 *  Created on: Jul 15, 2025
 *      Author: shakti
 */

#ifndef FMGL_FONTS_LOADABLE_INCLUDE_LOADABLE_FONT_PRIVATE_H_
#define FMGL_FONTS_LOADABLE_INCLUDE_LOADABLE_FONT_PRIVATE_H_

#include <stdint.h>

/**
 * Loadable font file header struct
 */
typedef struct
{
	/**
	 * Signature, must be FMGLFONT
	 */
	uint8_t Signature[8];

	/**
	 * File format version
	 */
	uint32_t Version;

	/**
	 * File contains this number of characters
	 */
	uint32_t CharactersCount;
}
FMGL_LoadableFont_FileHeaderStruct;


/**
 * Loadable font file character table item
 */
typedef struct
{
	/**
	 * Character code
	 */
	uint32_t Code;

	/**
	 * Offset to character data from beginning of file
	 */
	uint32_t Offset;
}
FMGL_LoadableFont_FileCharacterTableItemStruct;


/**
 * Loadable font file character data
 */
typedef struct
{
	/**
	 * Character raster width
	 */
	uint32_t Width;

	/**
	 * Character raster height
	 */
	uint32_t Height;

	/**
	 * Character raster size (in bytes)
	 */
	uint32_t RasterSize;

	/**
	 * Pointer to raster
	 */
	uint8_t* Raster;
}
FMGL_LoadableFont_FileCharacterDataStruct;

/**
 * Get character data by character code. Will not load raster
 */
FMGL_LoadableFont_FileCharacterDataStruct FMGL_LoadableFont_GetCharacterData(FMGL_LoadableFont_ContextStruct* context, uint8_t character);


#endif /* FMGL_FONTS_LOADABLE_INCLUDE_LOADABLE_FONT_PRIVATE_H_ */
