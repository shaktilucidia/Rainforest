/*
 * config_reader_writer.c
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#include "../../include/configuration/config_reader_writer.h"
#include "../../include/configuration/config_reader_writer_private.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* Try to find value in external memory located config */
char* ConfigGetValueByKey
(
	char* key,
	uint32_t baseAddress,
	uint32_t configSize,
	void* memoryDriverContext,
	void (*memoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
)
{
	uint32_t currentAddress = baseAddress;
	uint32_t totalToRead = configSize;

	char buffer[CONFIG_READ_BLOCK_SIZE];

	char* currentLine = malloc(1);
	uint32_t currentLineLength = 0;
	uint32_t bufferPosition;

	uint32_t toRead = 0;
	while (totalToRead > 0)
	{
		if (totalToRead > CONFIG_READ_BLOCK_SIZE)
		{
			toRead = CONFIG_READ_BLOCK_SIZE;
		}
		else
		{
			toRead = totalToRead;
		}

		memoryReadFunctionPtr(memoryDriverContext, currentAddress, toRead, (uint8_t*)buffer);

		totalToRead -= toRead;
		currentAddress += toRead;

		bufferPosition = 0;

		while (bufferPosition < toRead)
		{
			if ('\n' == buffer[bufferPosition])
			{
				/* Endline */
				currentLine[currentLineLength] = 0x00;

				if (IsKeyValuePairByKey(currentLine, key))
				{
					char* buffer = malloc(currentLineLength - strlen(key));
					ConfigGetValueFromPair(currentLine, key, buffer);

					free(currentLine);
					return buffer;
				}

				currentLineLength = 0;
			}
			else
			{
				currentLine = ConfigAddByteToArray(currentLine, currentLineLength, buffer[bufferPosition]);
				currentLineLength ++;
			}

			bufferPosition ++;
		}
	}

	currentLine[currentLineLength] = 0x00;

	if (IsKeyValuePairByKey(currentLine, key))
	{
		char* buffer = malloc(currentLineLength - strlen(key));
		ConfigGetValueFromPair(currentLine, key, buffer);

		free(currentLine);
		return buffer;
	}

	return NULL;
}

char* ConfigAddByteToArray(char* buffer, uint32_t size, char byteToAdd)
{
	buffer = realloc(buffer, size + 1);
	buffer[size] = byteToAdd;

	return buffer;
}

bool IsKeyValuePairByKey(char* buffer, char* key)
{
	return '#' != buffer[0] && 0 == strncmp(buffer, key, strlen(key));
}

void ConfigGetValueFromPair(char* buffer, char* key, char* result)
{
	uint32_t bufferLength = strlen(buffer);
	uint32_t keyLength = strlen(key);

	strncpy(result, (char*)&buffer[keyLength + 1], bufferLength - keyLength - 1);
	result[bufferLength - keyLength - 1] = 0x00;
}
