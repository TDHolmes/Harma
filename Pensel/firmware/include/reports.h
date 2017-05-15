#ifndef _REPORTS_H_
#define _REPORTS_H_

#include <stdint.h>
#include "common.h"
#include "LSM303DLHC.h"

#define OUTPUT_BUFF_LEN (255)


ret_t rpt_lookup(uint8_t rpt_type, uint8_t *input_buff_ptr, uint8_t input_buff_len,
                 uint8_t * output_buff_ptr, uint8_t * output_buff_len_ptr);

#endif /* _REPORTS_H_ */
