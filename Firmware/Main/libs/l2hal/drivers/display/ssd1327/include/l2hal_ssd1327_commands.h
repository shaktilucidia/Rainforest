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

#ifndef L2HAL_DRIVERS_DISPLAY_SSD1327_INCLUDE_L2HAL_SSD1327_COMMANDS_H_
#define L2HAL_DRIVERS_DISPLAY_SSD1327_INCLUDE_L2HAL_SSD1327_COMMANDS_H_

/* Column start and end address */
#define L2HAL_SSD1327_COMMAND_SET_COLUMN_ADDRESS 0x15
#define L2HAL_SSD1327_MAX_COLUMN_ADDRESS 0x3FU

/* Row start and end address */
#define L2HAL_SSD1327_COMMAND_SET_ROW_ADDRESS 0x75
#define L2HAL_SSD1327_MAX_ROW_ADDRESS 0x7FU

/* Set contrast control */
#define L2HAL_SSD1327_COMMAND_SET_CONTRAST 0x81

/* Set GDDRAM remap */
#define L2HAL_SSD1327_COMMAND_SET_REMAP 0xA0

#define L2HAL_SSD1327_ENABLE_COLUMN_ADDRESS_REMAP 0b1
#define L2HAL_SSD1327_ENABLE_NIBBLE_REMAP 0b10
#define L2HAL_SSD1327_ENABLE_VERTICAL_ADDRESS_INCREMENT 0b100
#define L2HAL_SSD1327_ENABLE_COM_REMAP 0b10000
#define L2HAL_SSD1327_ENABLE_COM_SPLIT_ODD_EVEN 0b1000000

/* Set display start line */
#define L2HAL_SSD1327_COMMAND_SET_DISPLAY_START_LINE 0xa1
#define L2HAL_SSD1327_MAX_START_LINE 127

/* Set display offset */
#define L2HAL_SSD1327_COMMAND_SET_VERTICAL_OFFSET 0xa2
#define L2HAL_SSD1327_MAX_VERTICAL_OFFSET 127

/* Set display mode */
#define L2HAL_SSD1327_COMMAND_SET_NORMAL_DISPLAY 0xa4
#define L2HAL_SSD1327_COMMAND_SET_ALL_ON 0xa5
#define L2HAL_SSD1327_COMMAND_SET_ALL_OFF 0xa6
#define L2HAL_SSD1327_COMMAND_SET_INVERSE 0xa7

/* Set MUX ratio*/
#define L2HAL_SSD1327_COMMAND_SET_MUX_RATIO 0xa8
#define L2HAL_SSD1327_MIN_MUXRATIO 16
#define L2HAL_SSD1327_MAX_MUXRATIO 128

/* Function selection A*/
#define L2HAL_SSD1327_COMMAND_SET_FUNCTION_A 0xab

/* On-off */
#define L2HAL_SSD1327_COMMAND_OFF 0xAE
#define L2HAL_SSD1327_COMMAND_ON 0xAF

/* Set phase length */
#define L2HAL_SSD1327_COMMAND_SET_PHASE_LENGTH 0xb1
#define L2HAL_SSD1327_MIN_PHASELENGTH 1U
#define L2HAL_SSD1327_MAX_PHASELENGTH 15U

/* No operation */
#define L2HAL_SSD1327_COMMAND_NOP 0xb2

/* Set front clock divider/ocsillator frequency */
#define L2HAL_SSD1327_COMMAND_SET_FCD_OF 0xb3
#define L2HAL_SSD1327_MIN_D 1
#define L2HAL_SSD1327_MAX_D 16
#define L2HAL_SSD1327_MAX_FOSC 15

/* Set GPIO */
#define L2HAL_SSD1327_COMMAND_SET_GPIO 0xb5

/* Set second pre-charge period */
#define L2HAL_SSD1327_COMMAND_SET_SECOND_PRECHARGE_PERIOD 0xb6
#define L2HAL_SSD1327_MIN_SECOND_PRECHARGE_PERIOD 0x01
#define L2HAL_SSD1327_MAX_SECOND_PRECHARGE_PERIOD 0x0F

/* Set grayscale table */
#define L2HAL_SSD1327_COMMAND_SET_GRAYSCALE_TABLE 0xb8
#define L2HAL_SSD1327_MAX_GRAYSCALE_LEVEL 63

/* Load default grayscale table */
#define L2HAL_SSD1327_COMMAND_SET_DEFAULT_GRAYSCALE_TABLE 0xb9

/* Set precharge voltage */
#define L2HAL_SSD1327_COMMAND_SET_PRECHARGE_VOLTAGE 0xbc
#define L2HAL_SSD1327_MAX_PRECHARGE_VOLTAGE 0x08

/* Set Vcomh */
#define L2HAL_SSD1327_COMMAND_SET_VCOMH 0xbe
#define L2HAL_SSD1327_VCOMH_MAX 0x07

/* Function select B */
#define L2HAL_SSD1327_COMMAND_SET_FUNCTION_B 0xd5
#define L2HAL_SSD1327_SET_FUNCTION_B_TEMPLATE 0b01100000
#define L2HAL_SSD1327_ENABLE_EXTERNAL_VSL 0b1
#define L2HAL_SSD1327_ENABLE_SECOND_PRECHARGE 0b10

/* Set command lock */
#define L2HAL_SSD1327_COMMAND_SET_COMMAND_LOCK 0xfd
#define L2HAL_SSD1327_LOCK_DISPLAY 0b100

#endif /* L2HAL_DRIVERS_DISPLAY_SSD1327_INCLUDE_L2HAL_SSD1327_COMMANDS_H_ */
