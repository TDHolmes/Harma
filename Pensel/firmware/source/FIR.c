/*!
 * @file    FIR.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    29-May-2017
 * @brief   Module for filtering arbitrary values given coefficients and a filter order.
 */
#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "FIR.h"

ret_t FIR_init(FIR_admin_t * FIR_ptr, uint16_t FIR_len, const float * coefficents_ptr)
{
    FIR_ptr = malloc(sizeof(FIR_admin_t));
    if (FIR_ptr == NULL) {
        return RET_NOMEM_ERR;
    }

    FIR_ptr->coefficents_ptr = coefficents_ptr;
    FIR_ptr->order = FIR_len;
    FIR_ptr->buffer = malloc(sizeof(float) * FIR_len); // TODO: Make circular?
    if (FIR_ptr->buffer == NULL) {
        return RET_NOMEM_ERR;
    }
    // initialize the memory
    for (int16_t i = FIR_ptr->order - 1; i >= 0; i--) {
        FIR_ptr->buffer[i] = 0.0f;
    }
    return RET_OK;
}


float FIR_run(FIR_admin_t * FIR_ptr, float new_val)
{
    float out_val = 0.0f;
    // Shift values through buffer. TODO: inefficient
    for (uint16_t i = FIR_ptr->order - 1; i > 0; i--) {
        FIR_ptr->buffer[i] = FIR_ptr->buffer[i - 1];
    }
    // input new item into filter
    FIR_ptr->buffer[0] = new_val;
    // calculate output value from filter!
    for (int16_t i = FIR_ptr->order - 1; i >= 0; i--) {
        out_val += FIR_ptr->coefficents_ptr[i] * FIR_ptr->buffer[i];
    }
    // TODO: combine for loops into single one
    return out_val;
}
