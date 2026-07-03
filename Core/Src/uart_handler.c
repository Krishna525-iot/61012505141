/*
 * uart_handler.c
 *
 *  Created on: Dec 26, 2024
 *      Author: Bheem
 */
/* ============================= */
/*       uart_handler.c          */
/* ============================= */

#include "uart_handler.h"
#include "globals.h"


static uint8_t rxBuffer[5];
static volatile uint8_t dataReady = 0;
#define PACKET_SIZE 5    // Define the expected packet size
static volatile uint8_t rx_index = 0;   // Index to track position in the buffer
//static volatile uint8_t int_flag = 0;  // Flag for additional logic (optional)

// UART Receive Complete Callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) // Check if it's the correct UART
    {
        uint8_t received_char = huart->Instance->RDR;  // Read the received character from RDR register

        // Reset buffer index if we receive the start of a new packet (when @ is detected)
        if (rx_index == 0 && received_char == '@') {
        	rxBuffer[rx_index++] = received_char;  // Store '@' in buffer as the start of a packet
        }
        else if (rx_index > 0) {
        	rxBuffer[rx_index++] = received_char;  // Store received character in buffer
        }

        // Check if the packet is complete (we receive the full packet size)
         if (rx_index == PACKET_SIZE) {
        	dataReady = 1;  // Set flag indicating the packet is ready
            rx_index = 0;  // Reset index for the next packet
//            int_flag = 1;   // Set an additional flag for further actions (optional)
        }
        else if (received_char != '@' && rx_index == 0) {
            // If we receive something other than '@' at the beginning, reset buffer
            rx_index = 0;
        }

        // Re-enable UART interrupt to continue receiving data
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&huart->Instance->RDR, 1);
    }
}

//uint8_t mockBuffer[5] = {'@', 'I', '0', '1', '#'};
//
//uint8_t* getRxBuffer(void) {
//    return mockBuffer; // Return simulated data instead of UART buffer
//}

uint8_t* getRxBuffer(void) {
     return rxBuffer;
}

uint8_t isDataReady(void) {
    return dataReady;
}

void resetDataReady(void) {
    dataReady = 0;
}
