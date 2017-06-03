/*
 *
 */

#ifndef _FIR_H_
#define _FIR_H_

#include <stdint.h>
#include "common.h"


typedef struct {
    const float * coefficents_ptr;
    uint16_t order;
    float * buffer;
} FIR_admin_t;


ret_t FIR_init(FIR_admin_t * FIR_ptr, uint16_t FIR_len, const float * coefficents_ptr);
float FIR_run(FIR_admin_t * FIR_ptr, float new_val);


#endif /* _FIR_H_ */
