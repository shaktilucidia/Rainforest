/*
 * config_reader_writer.c
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#include "../../include/configuration/config_reader_writer.h"
#include "../../include/configuration/config_reader_writer_private.h"
#include "../../include/filesystem.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "../libs/l2hal/include/l2hal_errors.h"

ConfigContextStruct ConfigLoad
(
	char* path,
	uint32_t baseAddress,
	void* memoryDriverContext,
	void (*memoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),
	void (*memoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
)
{
	ConfigContextStruct config;

	strcpy(config.Path, path);
	config.BaseAddress = baseAddress;
	config.MemoryDriverContext = memoryDriverContext;
	config.MemoryReadFunctionPtr = memoryReadFunctionPtr;
	config.MemoryWriteFunctionPtr = memoryWriteFunctionPtr;

	/* Actual load */
	config.ConfigSize = FS_LoadFileToExternalRam
	(
		config.Path,
		config.BaseAddress,
		config.MemoryDriverContext,
		config.MemoryWriteFunctionPtr
	);

	return config;
}

char* ConfigGetStringValueByKey
(
	ConfigContextStruct* context,
	char* key,
	bool* isFound
)
{
	uint32_t currentAddress = context->BaseAddress;
	uint32_t totalToRead = context->ConfigSize;

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

		context->MemoryReadFunctionPtr(context->MemoryDriverContext, currentAddress, toRead, (uint8_t*)buffer);

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
					char* result = malloc(currentLineLength - strlen(key));
					ConfigGetValueFromPair(currentLine, key, result);

					free(currentLine);

					*isFound = true;
					return result;
				}

				free(currentLine);
				currentLine = malloc(1);

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
		char* result = malloc(currentLineLength - strlen(key));
		ConfigGetValueFromPair(currentLine, key, result);

		free(currentLine);

		*isFound = true;
		return result;
	}

	free(currentLine);

	*isFound = false;
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

int32_t ConfigGetIntValueByKey
(
	ConfigContextStruct* context,
	char* key,
	bool* isFound
)
{
	char* valueRaw = ConfigGetStringValueByKey
	(
		context,
		key,
		isFound
	);

	if (!*isFound)
	{
		free(valueRaw);
		return 0;
	}

	int32_t result = 0;
	if (1 != sscanf(valueRaw, "%d", (int*)&result))
	{
		*isFound = false;
	}

	free(valueRaw);
	return result;
}

double ConfigGetDoubleValueByKey
(
	ConfigContextStruct* context,
	char* key,
	bool* isFound
)
{
	char* valueRaw = ConfigGetStringValueByKey
	(
		context,
		key,
		isFound
	);

	if (!*isFound)
	{
		free(valueRaw);
		return 0;
	}

	double result = 0.0;
	if (1 != sscanf(valueRaw, "%lf", (double*)&result))
	{
		*isFound = false;
	}

	free(valueRaw);
	return result;
}

void ConfigSetStringValueByKey
(
	ConfigContextStruct* context,
	char* key,
	char* value
)
{
	char tmpFilePath[256];
	sprintf(tmpFilePath, "%s_tmp", context->Path);

	FIL tmpFile;
	FRESULT fResult = f_open(&tmpFile, tmpFilePath, FA_CREATE_ALWAYS | FA_WRITE);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	uint32_t currentAddress = context->BaseAddress;
	uint32_t totalToRead = context->ConfigSize;

	char buffer[CONFIG_READ_BLOCK_SIZE];

	char* currentLine = malloc(1);
	uint32_t currentLineLength = 0;
	uint32_t bufferPosition;

	bool isKeyFound = false;

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

		context->MemoryReadFunctionPtr(context->MemoryDriverContext, currentAddress, toRead, (uint8_t*)buffer);


		totalToRead -= toRead;
		currentAddress += toRead;

		bufferPosition = 0;

		while (bufferPosition < toRead)
		{
			if ('\n' == buffer[bufferPosition])
			{
				/* Endline */
				currentLine[currentLineLength] = 0x00;

				if (ConfigProcessFileLineOnWrite(&tmpFile, currentLine, key, value))
				{
					isKeyFound = true;
				}

				free(currentLine);
				currentLine = malloc(1);

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

	if (0 != currentLineLength)
	{
		/* If last line is not empty */
		if (ConfigProcessFileLineOnWrite(&tmpFile, currentLine, key, value))
		{
			isKeyFound = true;
		}
	}

	free(currentLine);

	/* Key not found, adding */
	if (!isKeyFound)
	{
		ConfigWriteKeyValuePair(&tmpFile, key, value);
	}

	fResult = f_close(&tmpFile);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	/* Replacing file */
	fResult = f_unlink(context->Path);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	fResult = f_rename(tmpFilePath, context->Path);
	if (fResult != FR_OK)
	{
		L2HAL_Error(Generic);
	}

	/* Reload in memory */
	context->ConfigSize = FS_LoadFileToExternalRam
	(
		context->Path,
		context->BaseAddress,
		context->MemoryDriverContext,
		context->MemoryWriteFunctionPtr
	);
}

bool ConfigProcessFileLineOnWrite(FIL* fileToWrite, char* line, char* key, char* newValue)
{
	bool result;

	if (IsKeyValuePairByKey(line, key))
	{
		ConfigWriteKeyValuePair(fileToWrite, key, newValue);

		result = true;
	}
	else
	{
		f_puts(line, fileToWrite);

		result = false;
	}

	f_puts("\n", fileToWrite);

	return result;
}

void ConfigWriteKeyValuePair(FIL* fileToWrite, char* key, char* value)
{
	f_puts(key, fileToWrite);
	f_puts("=", fileToWrite);
	f_puts(value, fileToWrite);
}

void ConfigSetIntValueByKey
(
	ConfigContextStruct* context,
	char* key,
	int32_t value
)
{
	char buffer[32];
	sprintf(buffer, "%d", value);

	ConfigSetStringValueByKey(context, key, buffer);
}

void ConfigSetDoubleValueByKey
(
	ConfigContextStruct* context,
	char* key,
	double value
)
{
	char buffer[32];
	sprintf(buffer, "%lf", value);

	ConfigSetStringValueByKey(context, key, buffer);
}
