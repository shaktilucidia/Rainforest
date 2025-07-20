/*
 * console.c
 *
 *  Created on: Jul 20, 2025
 *      Author: Shakti
 */

#include "../include/console.h"
#include "../include/console_private.h"
#include <string.h>
#include "../../../include/l2hal_errors.h"

FMGL_Console_ContextStruct FMGL_ConsoleInit
(
	FMGL_API_DriverContext* fmglContext,
	FMGL_API_FontSettings* fontSettings
)
{
	FMGL_Console_ContextStruct context;

	context.FmglContext = fmglContext;
	context.FontSettings = fontSettings;

	memset(context.LinesBuffer, 0x00, FMGL_CONSOLE_LINES_BUFFER_SIZE * FMGL_CONSOLE_MAX_LINE_LENGTH);
	memset(context.LinesIndexes, 0x00, FMGL_CONSOLE_LINES_BUFFER_SIZE);

	context.LinesCount = 0;
	context.LinesBufferNewestLineIndex = 0;
	context.LinesBufferOldestLineIndex = 0;

	context.LinesFitToDisplay = FMGL_API_GetDisplayHeight(context.FmglContext) / (context.FontSettings->Font->Height * context.FontSettings->Scale);

	return context;
}

void FMGL_ConsoleAddLine
(
	FMGL_Console_ContextStruct* consoleContext,
	char* line
)
{
	if (strlen(line) >= FMGL_CONSOLE_MAX_LINE_LENGTH)
	{
		L2HAL_Error(Generic);
	}

	strcpy(consoleContext->LinesBuffer[consoleContext->LinesBufferNewestLineIndex], line);

	FMGL_ConsoleShiftIndexesUp(consoleContext);
	consoleContext->LinesIndexes[0] = consoleContext->LinesBufferNewestLineIndex;

	consoleContext->LinesBufferNewestLineIndex ++;
	consoleContext->LinesBufferNewestLineIndex %= FMGL_CONSOLE_LINES_BUFFER_SIZE;


	if (consoleContext->LinesCount < FMGL_CONSOLE_LINES_BUFFER_SIZE)
	{
		/* Add mode */
		consoleContext->LinesCount ++;
	}
	else
	{
		/* Scroll mode */
		consoleContext->LinesBufferOldestLineIndex = (consoleContext->LinesBufferNewestLineIndex + 1) % FMGL_CONSOLE_LINES_BUFFER_SIZE;
	}

	FMGL_ConsolePrint(consoleContext);
}

void FMGL_ConsoleShiftIndexesUp(FMGL_Console_ContextStruct* context)
{
	for (int16_t i = FMGL_CONSOLE_LINES_BUFFER_SIZE - 2; i >= 0; i --)
	{
		context->LinesIndexes[i + 1] = context->LinesIndexes[i];
	}
}

void FMGL_ConsolePrint(FMGL_Console_ContextStruct* consoleContext)
{
	FMGL_API_ClearScreen(consoleContext->FmglContext);

	uint16_t linePosition = 0;
	uint16_t width;
	uint16_t height;

	uint16_t linesCount = consoleContext->LinesFitToDisplay;
	if (consoleContext->LinesCount < linesCount)
	{
		linesCount = consoleContext->LinesCount;
	}

	for (int16_t line = linesCount - 1; line >= 0; line --)
	{
		FMGL_API_RenderTextWithLineBreaks
		(
			consoleContext->FmglContext,
			consoleContext->FontSettings,
			0,
			linePosition,
			&width,
			&height,
			false,
			consoleContext->LinesBuffer[consoleContext->LinesIndexes[line]]
		);

		linePosition += height;
	}

	FMGL_API_PushFramebuffer(consoleContext->FmglContext);
}
