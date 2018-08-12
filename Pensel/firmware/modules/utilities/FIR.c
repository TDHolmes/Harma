/*!
 * @file    FIR.c
 * @author  Tyler Holmes
 *
 * @date    29-May-2017
 * @brief   Module for filtering arbitrary values given coefficients and a filter order.
 */
#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "FIR.h"

/*! Initializes the given FIR admin pointer to be used in FIR_run.
 *
 * @param FIR_ptr (FIR_admin_t *): A pointer to an already allocated FIR_admin_t structure
 *      to be used in this initialization.
 * @param FIR_len (uint16_t): The length of the coefficients to be used
 * @param coefficents_ptr (const float *): The coefficients to be used in this filter
 * @return retval (ret_t): Success or failure reason of initializing the FIR structure
 */
ret_t FIR_init(FIR_admin_t * FIR_ptr, uint16_t FIR_len, const float * coefficents_ptr)
{
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

/*! Runs a cycle of the given FIR filter with a new value and returns the output value.
 *
 * @param FIR_ptr (FIR_admin_t *): A pointer to an already initialized
 *          FIR_admin_t structure
 * @param new_val (float): The new value to be added to the filter pipeline
 * @return out_val (float): The resulting value from the filter
 */
float FIR_run(FIR_admin_t * FIR_ptr, float new_val)
{
    float out_val = 0.0f;

    // input new item into filter
    FIR_ptr->buffer[0] = new_val;

    // Shift values through buffer while calculating output value from filter!
    for (int16_t i = FIR_ptr->order - 1; i >= 0; i--) {
        if (i != 0) {
            FIR_ptr->buffer[i] = FIR_ptr->buffer[i - 1];
        }
        out_val += FIR_ptr->coefficents_ptr[i] * FIR_ptr->buffer[i];
    }
    return out_val;
}
