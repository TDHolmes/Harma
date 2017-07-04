/*!
 * @file    UART.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Wrapper functions around the stm32f3xx HAL UART functions.
 *
 */
#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "common.h"


//! Timeout time for UART
#define UART_TIMEOUT_MS (50)  // can send 1150/2 chars in 50 miliseconds


ret_t UART_init(uint32_t baudrate);
bool UART_dataAvailable(void);
bool UART_TXisReady(void);
uint8_t UART_droppedPackets(void);

ret_t UART_sendData(uint8_t * data_ptr, uint8_t num_bytes);
ret_t UART_sendString(char string_ptr[]);
ret_t UART_sendint(int64_t data);
ret_t UART_sendfloat(float data, uint8_t percision);

ret_t UART_getChar(uint8_t * data_ptr);
ret_t UART_sendChar(uint8_t data);
ret_t UART_peakChar(uint8_t * data_ptr);


#endif /* _UART_H_ */
