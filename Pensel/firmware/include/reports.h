#ifndef _REPORTS_H_
#define _REPORTS_H_

#include <stdint.h>
#include "common.h"
#include "LSM303DLHC.h"

#define OUTPUT_BUFF_LEN (255)


ret_t rpt_init(ret_t (*putchr)(uint8_t), ret_t (*getchr)(uint8_t *));
ret_t rpt_run(void);

#endif /* _REPORTS_H_ */
