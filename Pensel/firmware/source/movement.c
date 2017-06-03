/*!
 * @file    movement.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's x/y/z movement through space via accel data.
 */
#include <stdint.h>

#include "LSM303DLHC.h"
#include "quanternions.h"

// TODO: band pass filter on raw accel data to get good movement readings
