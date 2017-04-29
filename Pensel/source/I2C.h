#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include "common.h"

ret_t I2C_init(void);
bool I2C_isBusy(void);
ret_t I2C_readData(uint8_t address, uint8_t mem_address, uint8_t * data_ptr, uint8_t data_len);
ret_t I2C_writeData(uint8_t dev_address, uint8_t mem_address, uint8_t * data_ptr, uint8_t data_len, bool blocking);
ret_t I2C_writeByte(uint8_t dev_address, uint8_t mem_address, uint8_t data, bool blocking);

#endif /* _I2C_H_ */
