/*!
 * @file    FIR.h
 * @author  Tyler Holmes
 *
 * @date    20-May-2017
 * @brief   Functions for running FIR filtering.
 */
#pragma once

#include <stdint.h>
#include "common.h"


typedef struct {
    const float * coefficents_ptr;
    uint16_t order;
    float * buffer;
} FIR_admin_t;


ret_t FIR_init(FIR_admin_t * FIR_ptr, uint16_t FIR_len, const float * coefficents_ptr);
float FIR_run(FIR_admin_t * FIR_ptr, float new_val);
