/*
 * hal.c
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#include "hal.h"
#include <stdint.h>
#include <string.h>

void HAL_InitHardware(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Info LED */
	L2HAL_MCU_ClockPortIn(HAL_INFO_LED_PORT);
	GPIO_InitStruct.Pin = HAL_INFO_LED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(HAL_INFO_LED_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(HAL_INFO_LED_PORT, HAL_INFO_LED_PIN, HAL_INFO_LED_INACTIVE_STATE);

	/* Initializing pSRAM pins */
	/* C/S */
	L2HAL_MCU_ClockPortIn(HAL_PSRAM_CS_PORT);
	GPIO_InitStruct.Pin = HAL_PSRAM_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(HAL_PSRAM_CS_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(HAL_PSRAM_CS_PORT, HAL_PSRAM_CS_PIN, GPIO_PIN_SET); /* 1 - Not selected */

	/* Initializing SD-card pins */
	/* C/S */
	L2HAL_MCU_ClockPortIn(HAL_SDCARD_CS_PORT);
	GPIO_InitStruct.Pin = HAL_SDCARD_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(HAL_SDCARD_CS_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(HAL_SDCARD_CS_PORT, HAL_SDCARD_CS_PIN, GPIO_PIN_SET); /* 1 - Not selected */

	/* Initializing display pins */

	/* Busy */
	L2HAL_MCU_ClockPortIn(HAL_DISPLAY_BUSY_PORT);
	GPIO_InitStruct.Pin = HAL_DISPLAY_BUSY_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(HAL_DISPLAY_BUSY_PORT, &GPIO_InitStruct);

	/* Reset */
	L2HAL_MCU_ClockPortIn(HAL_DISPLAY_RESET_PORT);
	GPIO_InitStruct.Pin = HAL_DISPLAY_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(HAL_DISPLAY_RESET_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(HAL_DISPLAY_RESET_PORT, HAL_DISPLAY_RESET_PIN, GPIO_PIN_RESET); /* 0 - keep resetted until driver start */

	/* Data / command */
	L2HAL_MCU_ClockPortIn(HAL_DISPLAY_DC_PORT);
	GPIO_InitStruct.Pin = HAL_DISPLAY_DC_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(HAL_DISPLAY_DC_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(HAL_DISPLAY_DC_PORT, HAL_DISPLAY_DC_PIN, GPIO_PIN_RESET); /* 0 - command  */

	/* CS */
	L2HAL_MCU_ClockPortIn(HAL_DISPLAY_CS_PORT);
	GPIO_InitStruct.Pin = HAL_DISPLAY_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(HAL_DISPLAY_CS_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(HAL_DISPLAY_CS_PORT, HAL_DISPLAY_CS_PIN, GPIO_PIN_SET); /* 1 - not selected  */
}

void HAL_SetInfoLedState(bool isLit)
{
	GPIO_PinState state;
	if (isLit)
	{
		state = HAL_INFO_LED_ACTIVE_STATE;
	}
	else
	{
		state = HAL_INFO_LED_INACTIVE_STATE;
	}

	HAL_GPIO_WritePin(L2HAL_ERROR_SIGNAL_PORT, L2HAL_ERROR_SIGNAL_PIN, state);
}

void HAL_HardwareSelfTest(void)
{
	// TODO: Uncomment me
	HAL_PSRAMSelfTest();
}

void HAL_PSRAMSelfTest(void)
{
	uint8_t writeBuffer[HAL_PSRAM_TEST_BUFFER_SIZE];
	for (uint16_t i = 0; i < HAL_PSRAM_TEST_BUFFER_SIZE; i++)
	{
		writeBuffer[i] = i % HAL_PSRAM_TEST_BUFFER_SIZE;
	}

	uint32_t blocksCounter = 0;
	for (uint32_t address = 0; address < L2HAL_LY68L6400_CAPACITY; address += HAL_PSRAM_TEST_BUFFER_SIZE)
	{
		HAL_SetInfoLedState((blocksCounter % (HAL_PSRAM_TEST_BLINK_BLOCKS_HALF_COUNT * 2)) < HAL_PSRAM_TEST_BLINK_BLOCKS_HALF_COUNT);

		L2HAL_LY68L6400_MemoryWrite(&RamContext, address, HAL_PSRAM_TEST_BUFFER_SIZE, writeBuffer);

		blocksCounter ++;
	}

	uint8_t readBuffer[HAL_PSRAM_TEST_BUFFER_SIZE];
	blocksCounter = 0;

	for (uint32_t address = 0; address < L2HAL_LY68L6400_CAPACITY; address += HAL_PSRAM_TEST_BUFFER_SIZE)
	{
		HAL_SetInfoLedState((blocksCounter % (HAL_PSRAM_TEST_BLINK_BLOCKS_HALF_COUNT * 2)) < HAL_PSRAM_TEST_BLINK_BLOCKS_HALF_COUNT);

		memset(readBuffer, 0, 0x00);
		L2HAL_LY68L6400_MemoryRead(&RamContext, address, HAL_PSRAM_TEST_BUFFER_SIZE, readBuffer);

		if (0 != memcmp(writeBuffer, readBuffer, HAL_PSRAM_TEST_BUFFER_SIZE))
		{
			L2HAL_Error(Generic);
		}

		blocksCounter ++;
	}

	HAL_SetInfoLedState(false);
}
