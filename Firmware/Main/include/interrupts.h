/*
 * interrupts.h
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#ifndef INCLUDE_INTERRUPTS_H_
#define INCLUDE_INTERRUPTS_H_

#include "../libs/l2hal/l2hal_config.h"

extern UART_HandleTypeDef UART1Handle;

/* Interrupts handlers */

/**
 * SysTick interrupt handler.
 */
void SysTick_Handler(void);

/* SPI1 DMA TX complete */
void DMA2_Stream3_IRQHandler(void);

/* SPI1 DMA RX complete */
void DMA2_Stream2_IRQHandler(void);

/* SPI2 DMA TX complete */
void DMA1_Stream4_IRQHandler(void);

/* SPI2 DMA RX complete */
void DMA1_Stream3_IRQHandler(void);

/* I2C1 */
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);

/* UART1 */
void USART1_IRQHandler(void);

#endif /* INCLUDE_INTERRUPTS_H_ */
