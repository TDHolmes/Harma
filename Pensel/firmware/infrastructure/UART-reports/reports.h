/*!
 * @file    reports.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Interface to define get/set reports as a debug hook.
 */
#pragma once

#include <stdint.h>
#include "common.h"
#include "LSM303DLHC.h"

#define OUTPUT_BUFF_LEN (255 + RPT_HEADER_SIZE)


ret_t rpt_init(ret_t (*putchr)(uint8_t), ret_t (*getchr)(uint8_t *));
ret_t rpt_sendStreamReport(uint8_t reportID, uint8_t payload_len, uint8_t * payload_ptr);
void rpt_run(void);
