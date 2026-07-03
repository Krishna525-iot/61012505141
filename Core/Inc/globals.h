/*
 * globals.h
 *
 *  Created on: Dec 26, 2024
 *      Author: Bheem
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

#include "main.h"

extern UART_HandleTypeDef huart1; // Declare huart1 as exter
#define RX_DATA_SIZE 5
extern uint8_t Rx_data[RX_DATA_SIZE];

#endif /* INC_GLOBALS_H_ */
