/*!
 * @file    reports.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Interface to define get/set reports as a debug hook.
 *
 */
#ifndef _REPORTS_H_
#define _REPORTS_H_

#include <stdint.h>
#include "common.h"
#include "LSM303DLHC.h"

#define OUTPUT_BUFF_LEN (255)


ret_t rpt_init(ret_t (*putchr)(uint8_t), ret_t (*getchr)(uint8_t *));
void rpt_run(void);

#endif /* _REPORTS_H_ */