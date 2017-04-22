#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include "common.h"

ret_t I2C_init(void);
bool I2C_isBusy(void);
ret_t I2C_read_byte(uint8_t I2C_address, uint8_t register_address, uint8_t * value_read_ptr);
ret_t I2C_read_bytes(uint8_t I2C_address, uint8_t register_address, uint8_t num_bytes, uint8_t * values_read_ptr);

ret_t I2C_write_byte(uint8_t I2C_address, uint8_t register_address, uint8_t value_to_write);
ret_t I2C_write_bytes(uint8_t I2C_address, uint8_t register_address, uint8_t num_vals, uint8_t * values_to_write_ptr);

#endif /* _I2C_H_ */
