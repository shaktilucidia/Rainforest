/*
 * console.h
 *
 *  Created on: Jul 20, 2025
 *      Author: Shakti
 */

#ifndef FMGL_CONSOLE_INCLUDE_CONSOLE_H_
#define FMGL_CONSOLE_INCLUDE_CONSOLE_H_

#include <stdint.h>
#include "../../include/fmgl.h"

/**
 * How many lines console will keep in buffer
 */
#define FMGL_CONSOLE_LINES_BUFFER_SIZE 64

/**
 * Maximal displayeable line length (remember that null-termination is a character too)
 */
#define FMGL_CONSOLE_MAX_LINE_LENGTH 64

/**
 * Console context
 */
typedef struct
{
	/**
	 * Console uses this FMGL context
	 */
	FMGL_API_DriverContext* FmglContext;

	/**
	 * Console font
	 */
	FMGL_API_FontSettings* FontSettings;

	/**
	 * Console lines are stored here
	 */
	char LinesBuffer[FMGL_CONSOLE_LINES_BUFFER_SIZE][FMGL_CONSOLE_MAX_LINE_LENGTH];

	/**
	 * Newest line index (in LinesBuffer)
	 */
	uint16_t LinesBufferNewestLineIndex;

	/**
	 * Oldest line index (in LinesBuffer)
	 */
	uint16_t LinesBufferOldestLineIndex;

	/**
	 * Lines indexes, like 0 -> newest line, 1 -> previous line and so on
	 */
	uint16_t LinesIndexes[FMGL_CONSOLE_LINES_BUFFER_SIZE];

	/**
	 * Lines count
	 */
	uint16_t LinesCount;

	/**
	 * How many lines will fit to screen
	 */
	uint16_t LinesFitToDisplay;

} FMGL_Console_ContextStruct;

/**
 * Initialize console
 */
FMGL_Console_ContextStruct FMGL_ConsoleInit
(
	FMGL_API_DriverContext* fmglContext,
	FMGL_API_FontSettings* fontSettings
);

/**
 * Add line to console
 */
void FMGL_ConsoleAddLine
(
	FMGL_Console_ContextStruct* consoleContext,
	char* line
);

/**
 * Print console context to screen (with clear and framebuffer push)
 */
void FMGL_ConsolePrint(FMGL_Console_ContextStruct* consoleContext);

#endif /* FMGL_CONSOLE_INCLUDE_CONSOLE_H_ */
