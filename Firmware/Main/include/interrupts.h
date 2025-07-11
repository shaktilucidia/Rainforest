/*
 * interrupts.h
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#ifndef INCLUDE_INTERRUPTS_H_
#define INCLUDE_INTERRUPTS_H_

#include "../libs/l2hal/l2hal_config.h"

/* Interrupts handlers */

/**
 * SysTick interrupt handler.
 */
volatile void SysTick_Handler(void);

/* SPI1 DMA TX complete */
volatile void DMA2_Stream3_IRQHandler(void);

/* SPI1 DMA RX complete */
volatile void DMA2_Stream2_IRQHandler(void);

/* SPI2 DMA TX complete */
volatile void DMA1_Stream4_IRQHandler(void);

/* SPI2 DMA RX complete */
volatile void DMA1_Stream3_IRQHandler(void);

#endif /* INCLUDE_INTERRUPTS_H_ */
