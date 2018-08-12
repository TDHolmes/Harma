/*!
 * @file    movement.h
 * @author  Tyler Holmes
 *
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's x/y/z movement through space via accel data.
 */
#pragma once

#include "quanternions.h"
#include <stdint.h>

// Lateral movement
typedef struct {
    float x;
    float y;
    float z;
} movement_t;
