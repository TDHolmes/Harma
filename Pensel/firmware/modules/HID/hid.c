/*
 *
 */
#include <string.h>

#include "common.h"
#include "peripherals/stm32-usb/usb_lib.h"
#include "peripherals/USB/usb_desc.h"

#include "modules/utilities/logging.h"
#include "modules/orientation/datatypes.h"
#include "modules/LSM9DS1/LSM9DS1.h"

#include "hid.h"

#define GEN_BUFF_SIZE             (0x40)
#define CLEAR_STATS_MAGIC_NUMBER  (0xAA)

extern critical_errors_t gCriticalErrors;

static uint8_t genBuff[GEN_BUFF_SIZE];


ret_t hid_getReport(report_id_t report_id, uint8_t payload_ptr[], uint8_t * payload_len_ptr)
{
    ret_t ret = RET_OK;
    accel_norm_t aPkt;
    mag_norm_t mPkt;
    gyro_norm_t gPkt;

    switch (report_id) {
        case kReportID_getAccelPacket:
            ret = LSM9DS1_getAccelPacket(&aPkt);
            if (ret == RET_OK) {
                memcpy(payload_ptr, &aPkt, sizeof(aPkt));
                *payload_len_ptr = sizeof(aPkt);
            } else {
                *payload_len_ptr = 0;
            }
            break;

        case kReportID_getMagPacket:
            ret = LSM9DS1_getMagPacket(&mPkt);
            if (ret == RET_OK) {
                memcpy(payload_ptr, &mPkt, sizeof(mPkt));
                *payload_len_ptr = sizeof(mPkt);
            } else {
                *payload_len_ptr = 0;
            }
            break;

        case kReportID_getGyroPacket:
            ret = LSM9DS1_getGyroPacket(&gPkt);
            if (ret == RET_OK) {
                memcpy(payload_ptr, &gPkt, sizeof(gPkt));
                *payload_len_ptr = sizeof(gPkt);
            } else {
                *payload_len_ptr = 0;
            }
            break;

        case kReportID_criticalErrors:
            memcpy(payload_ptr, &gCriticalErrors, sizeof(gCriticalErrors));
            *payload_len_ptr = sizeof(gCriticalErrors);
            ret = RET_OK;
            break;

        case kReportID_stringEcho:
            strncpy((char *)(payload_ptr), (char *)genBuff, sizeof(genBuff));
            *payload_len_ptr = strlen((char *)(payload_ptr));
            ret = RET_OK;
            break;

        case kReportID_helloWorld:
            strncpy((char *)(payload_ptr), "Hello!", sizeof(genBuff));
            *payload_len_ptr = strlen((char *)payload_ptr);
            ret = RET_OK;
            break;

        default:
            ret = RET_GEN_ERR;
            break;
    }

    LOG_MSG_FMT(kLogLevelInfo, "Get Report: 0x%02X (len: %i) ret: %i", report_id, *payload_len_ptr, ret);
    return ret;
}


ret_t hid_setReport(report_id_t report_id, uint8_t * payload_ptr, uint8_t payload_len)
{
    ret_t ret = RET_OK;

    switch (report_id) {
        case kReportID_getAccelPacket:
            ret = RET_GEN_ERR;
            break;

        case kReportID_getMagPacket:
            ret = RET_GEN_ERR;
            break;

        case kReportID_getGyroPacket:
            ret = RET_GEN_ERR;
            break;

        case kReportID_criticalErrors:
            if (payload_len == 2 && payload_ptr[1] == CLEAR_STATS_MAGIC_NUMBER) {
                memset(&gCriticalErrors, 0, sizeof(gCriticalErrors));
                ret = RET_OK;
            } else {
                ret = RET_GEN_ERR;
            }
            break;

        case kReportID_stringEcho:
            if (payload_len <= GEN_BUFF_SIZE) {
                strncpy((char *)genBuff, (char *)(payload_ptr + 1), payload_len);
                uint32_t genBuffLen = strlen((char *)genBuff) + 1;
                if (payload_len != genBuffLen) {
                    ret = RET_LEN_ERR;
                } else {
                    ret = RET_OK;
                }
            } else {
                ret = RET_MAX_LEN_ERR;
            }
            break;

        case kReportID_helloWorld:
            ret = RET_COM_ERR;
            break;

        default:
            ret = RET_GEN_ERR;
            break;
    }

    LOG_MSG_FMT(kLogLevelInfo, "Set Report: 0x%02X (len: %i) ret: %i", report_id, payload_len, ret);
    return ret;
}
