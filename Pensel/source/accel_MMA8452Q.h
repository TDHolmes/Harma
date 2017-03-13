#ifndef _ACCEL_DRIVER_H_
#define _ACCEL_DRIVER_H_

#include <stdint.h>
#include "common.h"

/*! Initializes the accel chip at the given address
 *
 * @param[in] accel_address (uint8_t): I2C address of the chip you want to initialize.
 * @param[out] retval (ret_t): Returns the error or pass status.
 */
ret_t accel_init(uint8_t accel_address);

/*! reads from the accelerometer the x, y and z data and normalizes it to 1 g.
 *
 * @param[in] accel_address (uint8_t): I2C address of the chip you want to read from.
 * @param[in] data_x_ptr (float *): Pointer to the location to store the x axis normalized value read.
 * @param[in] data_y_ptr (float *): Pointer to the location to store the y axis normalized value read.
 * @param[in] data_z_ptr (float *): Pointer to the location to store the z axis normalized value read.
 * @param[out] retval (ret_t): Returns the error or pass status.
 */
ret_t accel_getdata(uint8_t accel_address, float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);

#endif /**/
