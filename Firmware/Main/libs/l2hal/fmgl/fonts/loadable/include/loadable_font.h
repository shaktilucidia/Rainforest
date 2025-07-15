/*
 * loadable_font.h
 *
 *  Created on: Jul 15, 2025
 *      Author: shakti
 */

#ifndef FMGL_FONTS_LOADABLE_INCLUDE_LOADABLE_FONT_H_
#define FMGL_FONTS_LOADABLE_INCLUDE_LOADABLE_FONT_H_

#include <stdint.h>

/**
 * Loadable font context (characters data and so on are stored here)
 */
typedef struct
{
	/**
	 * Base address of unpacked font in memory (external or internal)
	 */
	uint32_t BaseAddress;

	/**
	 * Memory driver context pointer (usually it will be RAM-driver context)
	 */
	void* MemoryDriverContext;

	/**
	 * Pointer to function, writing to memory.
	 * 1st param - pointer to memory driver context (NOT to loadable font context)
	 * 2nd param - write start address
	 * 3rd param - write length
	 * 4rd param - pointer to buffer with data to write
	 */
	void (*MemoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*);

	/**
	 * Pointer to function, reading from memory.
	 * 1st param - pointer to memory driver context (NOT to loadable font context)
	 * 2nd param - read start address
	 * 3rd param - read length
	 * 4rd param - pointer to buffer where data will be readed
	 */
	void (*MemoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*);

	/**
	 * How much characters font have
	 */
	uint32_t CharactersCount;
}
FMGL_LoadableFont_ContextStruct;

/**
 * Load font from file
 */
void FMGL_LoadableFont_Init
(
	FMGL_LoadableFont_ContextStruct* context,

	char* path,

	void* memoryDriverContext,

	void (*memoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),

	void (*memoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),

	uint32_t baseAddress
);


/**
 * Get character width by character code
 */
uint16_t FMGL_LoadableFont_GetCharacterWidth(FMGL_LoadableFont_ContextStruct* context, uint8_t character);

#endif /* FMGL_FONTS_LOADABLE_INCLUDE_LOADABLE_FONT_H_ */
