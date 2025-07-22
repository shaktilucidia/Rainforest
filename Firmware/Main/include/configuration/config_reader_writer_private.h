/*
 * config_reader_writer_private.h
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_PRIVATE_H_
#define INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_PRIVATE_H_

#include <stdbool.h>
#include "../../include/filesystem.h"

/**
 * Add new byte to buffer, reallocating it and returning new address
 */
char* ConfigAddByteToArray(char* buffer, uint32_t size, char byteToAdd);

/**
 * Returns true if line contains key=value pair with given key and is not commented-out
 */
bool IsKeyValuePairByKey(char* buffer, char* key);

/**
 * Result value if buffer contains key=value pair, otherwise \0
 */
void ConfigGetValueFromPair(char* buffer, char* key, char* result);

/**
 * Copy line to file if key not found in line, or write key=newValue pair to file if found
 * Returns true if key found
 */
bool ConfigProcessFileLineOnWrite(FIL* fileToWrite, char* line, char* key, char* newValue);

/**
 * Write key=value pair to file (no newline!)
 */
void ConfigWriteKeyValuePair(FIL* fileToWrite, char* key, char* value);



#endif /* INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_PRIVATE_H_ */
