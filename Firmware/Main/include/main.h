/*
 * main.h
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#ifndef INCLUDE_MAIN_H_
#define INCLUDE_MAIN_H_

#include "../libs/l2hal/l2hal_config.h"
#include "hal.h"
#include "filesystem.h"
#include "localization/localizator.h"
#include "configuration/config_reader_writer.h"
#include "constants/generic.h"
#include "constants/paths.h"
#include "constants/addresses.h"

/**
 * Called every SysTick, executed in interrupt context
 */
void OnSysTick(void);



#endif /* INCLUDE_MAIN_H_ */
