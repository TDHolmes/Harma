/*!
 * @file    movement.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's x/y/z movement through space via accel data.
 */
#pragma once

#include <stdint.h>

#include "quanternions.h"


// Lateral movement
typedef struct {
    float x;
    float y;
    float z;
} movement_t;
