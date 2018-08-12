/*
 *
 */

#pragma once

#include "common.h"
#include <stdbool.h>

typedef enum {
    kReportID_getAccelPacket = 0x40,
    kReportID_getMagPacket = 0x41,
    kReportID_getGyroPacket = 0x42,
    kReportID_criticalErrors = 0x7F,
    kReportID_stringEcho = 0xf0,
    kReportID_helloWorld = 0xf1,
} report_id_t;

ret_t hid_getReport(report_id_t report_id, uint8_t payload_ptr[], uint8_t *payload_len_ptr);
ret_t hid_setReport(report_id_t report_id, uint8_t *payload_ptr, uint8_t payload_len);
