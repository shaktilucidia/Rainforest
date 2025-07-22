/*
 * interrupts.c
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#include "interrupts.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	L2HAL_SysTick_Callback();
}

/* SPI1 DMA TX complete */
void DMA2_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SPI1Handle.hdmatx);

	L2HAL_PSRAMDmaCompleted(SPI1Handle.hdmatx);

	L2HAL_SDCardDmaCompleted(SPI1Handle.hdmatx);
}

/* SPI1 DMA RX complete */
void DMA2_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SPI1Handle.hdmarx);

	L2HAL_PSRAMDmaCompleted(SPI1Handle.hdmatx);

	L2HAL_SDCardDmaCompleted(SPI1Handle.hdmatx);
}

/* SPI2 DMA TX complete */
void DMA1_Stream4_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SPI2Handle.hdmatx);

	L2HAL_DisplayDmaCompleted(SPI2Handle.hdmatx);
}

/* SPI2 DMA RX complete */
void DMA1_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SPI2Handle.hdmarx);
}

void I2C1_EV_IRQHandler(void)
{
	HAL_I2C_EV_IRQHandler(&I2C1_Handle);
}

void I2C1_ER_IRQHandler(void)
{
	HAL_I2C_ER_IRQHandler(&I2C1_Handle);
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&UART1Handle);
}
