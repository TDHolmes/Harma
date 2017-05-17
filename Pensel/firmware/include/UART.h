#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "common.h"


ret_t UART_init(uint32_t baudrate);
bool UART_dataAvailable(void);
bool UART_TXisReady(void);
uint8_t UART_droppedPackets(void);

ret_t UART_sendData(uint8_t * data_ptr, uint8_t num_bytes);
ret_t UART_sendChar(uint8_t data);
ret_t UART_sendString(char string_ptr[]);
ret_t UART_sendint(int64_t data);
ret_t UART_sendfloat(float data);

ret_t UART_getChar(uint8_t * data_ptr);
ret_t UART_peakChar(uint8_t * data_ptr);


#endif /* _UART_H_ */
