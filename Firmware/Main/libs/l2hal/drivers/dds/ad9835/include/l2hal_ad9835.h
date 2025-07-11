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

/**
 * @file
 * @brief AD9835 DDS level 2 HAL driver.
 * How to use this driver:
 *
 * 1) Set-up SPI using STM32 HAL.
 *    Sample code:
 *
 *	void L2HAL_SetupSPI(void)
 *	{
 *		SPIHandle.Instance				 = SPI1;
 *		SPIHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
 *		SPIHandle.Init.Direction         = SPI_DIRECTION_2LINES;
 *		SPIHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
 *		SPIHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
 *		SPIHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
 *		SPIHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
 *		SPIHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
 *		SPIHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
 *		SPIHandle.Init.CRCPolynomial     = 0;
 *		SPIHandle.Init.NSS               = SPI_NSS_SOFT;
 *		SPIHandle.Init.Mode = SPI_MODE_MASTER;
 *
 *		if(HAL_SPI_Init(&SPIHandle) != HAL_OK)
 *		{
 *			L2HAL_Error(Generic);
 *		}
 *	}
 *
 * 2) Create an instance of L2HAL_AD9835_ContextStruct struct and fill it with data, then call L2HAL_AD9835_Init():
 *
 * L2HAL_AD9835_Context.SPIHandle = &SPIHandle;
 * L2HAL_AD9835_Context.FSYNCPort = GPIOA;
 * L2HAL_AD9835_Context.FSYNCPin = GPIO_PIN_8;
 * L2HAL_AD9835_Init(&L2HAL_AD9835_Context);
 *
 * 3) For example, specify desired frequency using (default) FREG0:
 *
 * L2HAL_AD9835_WriteFrequencyWord(&L2HAL_AD9835_Context, Freg0, frequency);
 */

#ifndef L2HAL_DRIVERS_DDS_AD9835_INCLUDE_L2HAL_AD9835_H_
#define L2HAL_DRIVERS_DDS_AD9835_INCLUDE_L2HAL_AD9835_H_

#include <stdbool.h>
#include "../../../../include/l2hal_aux.h"
#include "../include/l2hal_ad9835_private.h"

/**
 * Initialize AD9835.
 */
void L2HAL_AD9835_Init(L2HAL_AD9835_ContextStruct* context);

/**
 * Select source for PHASE and FREQ.
 * @sync FSEL/PSEL synchronous load.
 * @useBits If true, then use control bits, otherwise - control pins.
 */
void L2HAL_AD9835_SelectPFSource(L2HAL_AD9835_ContextStruct* context, bool sync, bool useBits);

/**
 * Power control and reset.
 * @sleep If true, then DDS is put to sleep.
 * @reset Reset phase accumulator.
 * @clear Reset SYNC and SELSRC.
 */
void L2HAL_AD9835_PowerControl(L2HAL_AD9835_ContextStruct* context, bool sleep, bool reset, bool clear);

/**
 * Select given phase register (if bits used as registers sources).
 */
void L2HAL_AD9835_SelectPhaseRegister(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_PhaseRegisters phaseRegister);

/**
 * Select given frequency register (if bits used as registers sources).
 */
void L2HAL_AD9835_SelectFrequencyRegister(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_FrequencyRegisters frequencyRegister);

/**
 * As L2HAL_AD9835_SelectPhaseRegister() and L2HAL_AD9835_SelectFrequencyRegister(), but sets both registers using only one write
 */
void L2HAL_AD9835_SelectFrequencyAndPhaRegisters(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_FrequencyRegisters frequencyRegister,
		enum L2HAL_AD9835_PhaseRegisters phaseRegister);

/**
 * Writes given 8 frequency bits to defer register. Feed with destination frequency register and data.
 */
void L2HAL_AD9835_Write8FrequencyBitsToDeferRegister(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_Registers frequencyRegister, uint8_t data);

/**
 * Writes 8 given bits + 8 bits from defer register into given frequency register.
 */
void L2HAL_AD9835_Write16FrequencyBits(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_Registers frequencyRegister, uint8_t data);

/**
 * As L2HAL_AD9835_Write8FrequencyBitsToDeferRegister(), but for phase register
 */
void L2HAL_AD9835_Write8PhaseBitsToDeferRegister(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_Registers phaseRegister, uint8_t data);

/**
 * As L2HAL_AD9835_Write16FrequencyBits(), but for phase register
 */
void L2HAL_AD9835_Write16PhaseBits(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_Registers phaseRegister, uint8_t data);

/**
 * Write full frequency word into given frequency register.
 */
void L2HAL_AD9835_WriteFrequencyWord(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_FrequencyRegisters frequencyRegister, uint32_t data);

/**
 * As L2HAL_AD9835_WriteFrequencyWord(), but for phase register.
 */
void L2HAL_AD9835_WritePhaseWord(L2HAL_AD9835_ContextStruct* context, enum L2HAL_AD9835_PhaseRegisters phaseRegister, uint16_t data);

#endif /* L2HAL_DRIVERS_DDS_AD9835_INCLUDE_L2HAL_AD9835_H_ */
