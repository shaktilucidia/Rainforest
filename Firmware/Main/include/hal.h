/*
 * hal.h
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#ifndef INCLUDE_HAL_H_
#define INCLUDE_HAL_H_

#include "../libs/l2hal/l2hal_config.h"
#include <stdbool.h>

/* Info LED */
#define HAL_INFO_LED_PORT GPIOC
#define HAL_INFO_LED_PIN GPIO_PIN_13
#define HAL_INFO_LED_INACTIVE_STATE GPIO_PIN_SET
#define HAL_INFO_LED_ACTIVE_STATE GPIO_PIN_RESET

/**********************
 *  pSRAM - LY68L6400 *
 *
 *  SPI 1
 *  PB4 - CS (Active 0)
 *
 **********************/
#define HAL_PSRAM_CS_PORT GPIOB
#define HAL_PSRAM_CS_PIN GPIO_PIN_4

#define HAL_PSRAM_TEST_BUFFER_SIZE 256
#define HAL_PSRAM_TEST_BLINK_BLOCKS_HALF_COUNT 512

/**********************
 *  SD CARD *
 *
 *  SPI 1
 *  PB5 - CS (Active 0)
 *
 **********************/

#define HAL_SDCARD_CS_PIN GPIO_PIN_5
#define HAL_SDCARD_CS_PORT GPIOB

/**********************
 *  Display - SSD1683 *
 *
 *  SPI 2
 *  PB0 - Busy (active 1)
 *  PB1 - Reset (active 0)
 *  PB2 - Data (1) / Command (0)
 *  PB3 - CS (active 0)
 *
 **********************/
#define HAL_DISPLAY_BUSY_PORT GPIOB
#define HAL_DISPLAY_BUSY_PIN GPIO_PIN_0

#define HAL_DISPLAY_RESET_PORT GPIOB
#define HAL_DISPLAY_RESET_PIN GPIO_PIN_1

#define HAL_DISPLAY_DC_PORT GPIOB
#define HAL_DISPLAY_DC_PIN GPIO_PIN_2

#define HAL_DISPLAY_CS_PORT GPIOB
#define HAL_DISPLAY_CS_PIN GPIO_PIN_3


/**
 * Bluetooth full speed
 */
#define HAL_BLUETOOTH_FULL_SPEED_BAUDRATE 115200

/**
 * Bluetooth factory speed
 */
#define HAL_BLUETOOTH_FACTORY_SPEED_BAUDRATE 9600


extern L2HAL_LY68L6400_ContextStruct RamContext;
extern UART_HandleTypeDef UART1Handle;

/**
 * Init project-specific hardware here
 */
void HAL_InitHardware(void);

/* Info led on / off */
void HAL_SetInfoLedState(bool isLit);

/* Call this for hardware self-test */
void HAL_HardwareSelfTest(void);

/**
 * Test attached PSRAM
 */
void HAL_PSRAMSelfTest(void);

/**
 * Initialize UART1 at given speed
 */
void HAL_UART1_Init(uint32_t baudrate);

/**
 * Deinitialize UART1
 */
void HAL_UART1_DeInit(void);



#endif /* INCLUDE_HAL_H_ */
