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
 * @brief Command constants for SSD1306 OLED display controller.
 * Only needed commands are here, see datasheet for full list.
 * When words like "flipped" / "mirrored" used, they are applied to cheap Chinese OLEDs based on SSD1306.
 */

#ifndef L2HAL_INCLUDE_SSD1306HAL_L2HAL_SSD1306_COMMANDS_C_
#define L2HAL_INCLUDE_SSD1306HAL_L2HAL_SSD1306_COMMANDS_C_

/* On-off */
#define L2HAL_SSD1306_COMMAND_ON 0xAF /* Turn display ON */
#define L2HAL_SSD1306_COMMAND_OFF 0xAE /* Turn display OFF */

/* Memory addressing mode */
#define L2HAL_SSD1306_COMMAND_SET_MEMORY_ADDRESSING_MODE 0x20 /* Set memory addressing mode */
#define L2HAL_SSD1306_COMMAND_ADDRESSING_MODE_HORIZONAL 0x00 /* Horizontal mode */
#define L2HAL_SSD1306_COMMAND_ADDRESSING_MODE_VERTICAL 0x01 /* Vertical mode */
#define L2HAL_SSD1306_COMMAND_ADDRESSING_MODE_PAGED 0x02 /* Paged mode */

/* COM output scan direction (vertical flipping) */
#define L2HAL_SSD1306_COMMAND_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL 0xC0 /* Flipped */
#define L2HAL_SSD1306_COMMAND_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED 0xC8 /* Normal */

/* Segments remap (horizontal flip) */
#define L2HAL_SSD1306_COMMAND_SET_SEGMENT_REMAP_NORMAL 0xA0 /* Flipped */
#define L2HAL_SSD1306_COMMAND_SET_SEGMENT_REMAP_REMAPPED 0xA1 /* Normal */

/* Column start and end addresses (left and right borders) */
#define L2HAL_SSD1306_COMMAND_SET_COLUMN_ADDRESS 0x21
#define L2HAL_SSD1306_COMMAND_COLUMN_ADDRESS_LEFTMOST 0x00
#define L2HAL_SSD1306_COMMAND_COLUMN_ADDRESS_RIGHTMOST 0x7F /* 128 pixels width display */

/* Page start and end addresses (top and bottom border pages) */
#define L2HAL_SSD1306_COMMAND_SET_PAGE_ADDRESS 0x22
#define L2HAL_SSD1306_COMMAND_PAGE_ADDRESS_TOPMOST 0x00
#define L2HAL_SSD1306_COMMAND_PAGE_ADDRESS_BOTTOMMOST 0x07

/* Start line (possible 0x40 - 0x7F) */
#define L2HAL_SSD1306_COMMAND_SET_START_LINE_0 0x40 /* Start from line 0 */
#define L2HAL_SSD1306_COMMAND_SET_START_LINE_63 0x7F /* Start from line 63 */

/* Pixels brightness inversion */
#define L2HAL_SSD1306_COMMAND_SET_PIXELS_NORMAL 0xA6 /* Normal (0 is off) */
#define L2HAL_SSD1306_COMMAND_SET_PIXELS_INVERTED 0xA7 /* Inverted (0 is on) */

/* VRAM ignore*/
#define L2HAL_SSD1306_COMMAND_SET_VRAM_IGNORE_NO 0xA4 /* Image follows VRAM contents */
#define L2HAL_SSD1306_COMMAND_SET_VRAM_IGNORE_YES 0xA5 /* Whole display on */

/* Set multiplex ratio (15 - 63 allowed) */
#define L2HAL_SSD1306_COMMAND_SET_MULTIPLEX_RATIO 0xA8
#define L2HAL_SSD1306_COMMAND_MULTIPLEX_RATIO_16 0x0F /* MUX 16 */
#define L2HAL_SSD1306_COMMAND_MULTIPLEX_RATIO_64 0x3F /* MUX 64 */

/* Set vertical display offset */
#define L2HAL_SSD1306_COMMAND_SET_DISPLAY_OFFSET 0xD3
#define L2HAL_SSD1306_COMMAND_DISPLAY_OFFSET_0 0x00
#define L2HAL_SSD1306_COMMAND_DISPLAY_OFFSET_63 0x3F

/* Set display frequency */
#define L2HAL_SSD1306_COMMAND_SET_DISPLAY_FREQUENCY 0xD5
#define L2HAL_SSD1306_COMMAND_DISPLAY_FREQUENCY_MAX_DIVIDE_1 0xF0

/* Set precharge periods */
#define L2HAL_SSD1306_COMMAND_SET_PRECHARGE_PERIODS 0xD9
#define L2HAL_SSD1306_COMMAND_PRECHARGE_PERIODS_DEFAULT 0x22

/* Set COM pins hardware configuration */
#define L2HAL_SSD1306_COMMAND_SET_COM_PINS_HW_CONFIG 0xDA
#define L2HAL_SSD1306_COMMAND_COM_PINS_HW_CONFIG_SEQ_NO_REMAP 0x02
#define L2HAL_SSD1306_COMMAND_COM_PINS_HW_CONFIG_ALT_NO_REMAP 0x12
#define L2HAL_SSD1306_COMMAND_COM_PINS_HW_CONFIG_SEQ_REMAP 0x22
#define L2HAL_SSD1306_COMMAND_COM_PINS_HW_CONFIG_ALT_REMAP 0x32

/* Set Vcomh deselect level*/
#define L2HAL_SSD1306_COMMAND_SET_VCOMH_LEVEL 0xDB
#define L2HAL_SSD1306_COMMAND_VCOMH_LEVEL_065 0x00 /* 0.65*Vcc */
#define L2HAL_SSD1306_COMMAND_VCOMH_LEVEL_077 0x20 /* 0.77*Vcc */
#define L2HAL_SSD1306_COMMAND_VCOMH_LEVEL_083 0x30 /* 0.83*Vcc */

/* Set Vcc step-up mode */
#define L2HAL_SSD1306_COMMAND_SET_VCC_MODE 0x8D
#define L2HAL_SSD1306_COMMAND_VCC_MODE_OFF 0x10 /* Vcc step-up disabled */
#define L2HAL_SSD1306_COMMAND_VCC_MODE_ON 0x14 /* Vcc step-up enabled */

/* Set brightness level (0x00 (min) - 0xFF (max)) */
#define L2HAL_SSD1306_COMMAND_SET_BRIGHTNESS_LEVEL 0x81

#endif /* L2HAL_INCLUDE_SSD1306HAL_L2HAL_SSD1306_COMMANDS_C_ */
