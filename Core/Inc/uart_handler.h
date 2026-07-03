/*
 * uart_handler.h
 *
 *  Created on: Dec 26, 2024
 *      Author: Bheem
 */
/* ============================= */
/*       uart_handler.h          */
/* ============================= */
#ifndef INC_UART_HANDLER_H_
#define INC_UART_HANDLER_H_


#include <stdint.h>

uint8_t isDataReady(void);
void resetDataReady(void);
uint8_t* getRxBuffer(void);

#endif /* INC_UART_HANDLER_H_ */
