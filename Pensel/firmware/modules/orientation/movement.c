/*!
 * @file    movement.c
 * @author  Tyler Holmes
 *
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's x/y/z movement through space via accel data.
 */
#include <stdint.h>

#include "quanternions.h"
#include "FIR_coefficients.h"


// TODO: band pass filter on raw accel data to get good movement readings
