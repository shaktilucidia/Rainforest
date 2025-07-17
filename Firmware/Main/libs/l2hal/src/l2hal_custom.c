/*
	This file is part of Shakti Lucidia's STM32 level 2 HAL.

	STM32 level 2 HAL is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	-------------------------------------------------------------------------

	Created by Shakti Lucidia

	Feel free to contact: shakti_lucidia@proton.me

	Repository: https://github.com/shaktilucidia/stm32-l2hal

	-------------------------------------------------------------------------
 */

#include "../include/l2hal_custom.h"

void L2HAL_InitCustomHardware(void)
{
	L2HAL_SetupSPI();
	L2HAL_SetupI2C();
}

/* Begin SPI-related stuff */

void L2HAL_SetupSPI(void)
{
	/* SPI1 */
	SPI1Handle.Instance = SPI1;
	SPI1Handle.Init.Mode = SPI_MODE_MASTER;
	SPI1Handle.Init.Direction = SPI_DIRECTION_2LINES;
	SPI1Handle.Init.DataSize =  SPI_DATASIZE_8BIT;
	SPI1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	SPI1Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI1Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI1Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI1Handle.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI1Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI1Handle.Init.NSS = SPI_NSS_SOFT;

	if(HAL_SPI_Init(&SPI1Handle) != HAL_OK)
	{
		/* Initialization Error */
		L2HAL_Error(Generic);
	}

	/* SPI2 */
	SPI2Handle.Instance = SPI2;
	SPI2Handle.Init.Mode = SPI_MODE_MASTER;
	SPI2Handle.Init.Direction = SPI_DIRECTION_2LINES;
	SPI2Handle.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	SPI2Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI2Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI2Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI2Handle.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI2Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI2Handle.Init.NSS = SPI_NSS_SOFT;

	if(HAL_SPI_Init(&SPI2Handle) != HAL_OK)
	{
		/* Initialization Error */
		L2HAL_Error(Generic);
	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI1)
	{
		/**
		 * SPI1 at PA5 (SCK), PA6 (MISO) and PA7 (MOSI)
		 */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_RCC_DMA2_CLK_ENABLE();

		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* TX DMA */
		SPI1TxDmaHandle.Instance = DMA2_Stream3;
		SPI1TxDmaHandle.Init.Channel = DMA_CHANNEL_3;
		SPI1TxDmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
		SPI1TxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
		SPI1TxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
		SPI1TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		SPI1TxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		SPI1TxDmaHandle.Init.Mode = DMA_NORMAL;
		SPI1TxDmaHandle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		SPI1TxDmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		SPI1TxDmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		SPI1TxDmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;
		SPI1TxDmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;

		if (HAL_DMA_Init(&SPI1TxDmaHandle) != HAL_OK)
		{
			L2HAL_Error(Generic);
		}

		__HAL_LINKDMA(hspi, hdmatx, SPI1TxDmaHandle);

		HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

		/* RX DMA */
		SPI1RxDmaHandle.Instance = DMA2_Stream2;
		SPI1RxDmaHandle.Init.Channel = DMA_CHANNEL_3;
		SPI1RxDmaHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
		SPI1RxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
		SPI1RxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
		SPI1RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		SPI1RxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		SPI1RxDmaHandle.Init.Mode = DMA_NORMAL;
		SPI1RxDmaHandle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		SPI1RxDmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		SPI1RxDmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		SPI1RxDmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;
		SPI1RxDmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;

		if (HAL_DMA_Init(&SPI1RxDmaHandle) != HAL_OK)
		{
			L2HAL_Error(Generic);
		}

		__HAL_LINKDMA(hspi, hdmarx, SPI1RxDmaHandle);

		HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	}

	if (hspi->Instance == SPI2)
	{
		/**
		 * SPI2 at PB13 (SCK), PB14 (MISO) PB15 (MOSI)
		 */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_SPI2_CLK_ENABLE();
		__HAL_RCC_DMA1_CLK_ENABLE();

		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

		GPIO_InitStruct.Pin = GPIO_PIN_13;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_14;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_15;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* TX DMA */
		SPI2TxDmaHandle.Instance = DMA1_Stream4;
		SPI2TxDmaHandle.Init.Channel = DMA_CHANNEL_0;
		SPI2TxDmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
		SPI2TxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
		SPI2TxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
		SPI2TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		SPI2TxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		SPI2TxDmaHandle.Init.Mode = DMA_NORMAL;
		SPI2TxDmaHandle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		SPI2TxDmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		SPI2TxDmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		SPI2TxDmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;
		SPI2TxDmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;

		if (HAL_DMA_Init(&SPI2TxDmaHandle) != HAL_OK)
		{
			L2HAL_Error(Generic);
		}

		__HAL_LINKDMA(hspi, hdmatx, SPI2TxDmaHandle);

		HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

		/* RX DMA */
		SPI2RxDmaHandle.Instance = DMA1_Stream3;
		SPI2RxDmaHandle.Init.Channel = DMA_CHANNEL_0;
		SPI2RxDmaHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
		SPI2RxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
		SPI2RxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
		SPI2RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		SPI2RxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		SPI2RxDmaHandle.Init.Mode = DMA_NORMAL;
		SPI2RxDmaHandle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		SPI2RxDmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		SPI2RxDmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		SPI2RxDmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;
		SPI2RxDmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;

		if (HAL_DMA_Init(&SPI2RxDmaHandle) != HAL_OK)
		{
			L2HAL_Error(Generic);
		}

		__HAL_LINKDMA(hspi, hdmarx, SPI2RxDmaHandle);

		HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI1)
	{
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

		HAL_DMA_DeInit(&SPI1TxDmaHandle);
		HAL_NVIC_DisableIRQ(DMA2_Stream3_IRQn);

		HAL_DMA_DeInit(&SPI1RxDmaHandle);
		HAL_NVIC_DisableIRQ(DMA2_Stream2_IRQn);
	}

	if (hspi->Instance == SPI2)
	{
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

		HAL_DMA_DeInit(&SPI2TxDmaHandle);
		HAL_NVIC_DisableIRQ(DMA1_Stream4_IRQn);

		HAL_DMA_DeInit(&SPI2RxDmaHandle);
		HAL_NVIC_DisableIRQ(DMA1_Stream3_IRQn);
	}
}

void L2HAL_PSRAMDmaCompleted(DMA_HandleTypeDef *hdma)
{
	L2HAL_LY68L6400_MarkDataTransferAsCompleted(&RamContext);
}

void L2HAL_DisplayDmaCompleted(DMA_HandleTypeDef *hdma)
{
	L2HAL_SSD1683_MarkDataTransferAsCompleted(&DisplayContext);
}

void L2HAL_SDCardDmaCompleted(DMA_HandleTypeDef *hdma)
{
	L2HAL_SDCard_MarkDataTransferAsCompleted(&SDCardContext);
}

void L2HAL_SetupI2C(void)
{
	/* I2C1 */
	__HAL_RCC_I2C1_CLK_ENABLE();

	I2C1_Handle.Instance = I2C1;
	I2C1_Handle.Init.ClockSpeed = 10000;
	I2C1_Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	I2C1_Handle.Init.OwnAddress1 = 0x00;
	I2C1_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2C1_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C1_Handle.Init.OwnAddress2 = 0x00;
	I2C1_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C1_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if(HAL_I2C_Init(&I2C1_Handle) != HAL_OK)
	{
		/* Initialization Error */
		L2HAL_Error(Generic);
	}
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == I2C1)
	{
		/**
		 * I2C1 at PB8 (SCL) and PB9 (SDA)
		 */

		/* Clocking port */
		__HAL_RCC_GPIOB_CLK_ENABLE();

		GPIO_InitTypeDef GPIO_InitStruct;

		GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;

		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Display driver uses I2C interrupts exchange */
		HAL_NVIC_SetPriority(I2C1_ER_IRQn, 15, 0);
		HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
		HAL_NVIC_SetPriority(I2C1_EV_IRQn, 15, 0);
		HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
	if (hi2c->Instance == I2C1)
	{
		HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
		HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);

		__HAL_RCC_I2C1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
	}
}


