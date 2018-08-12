/*!
 * @file    cal.h
 * @author  Tyler Holmes
 *
 * @date    20-May-2017
 * @brief   Calibration code and structures
 */
#pragma once

#include <stdint.h>
#include "common.h"


#define PENSEL_CAL_HEADER (0x9f5366f1)  //!< Randomly generated cal header...
#define PENSEL_CAL_VERSION (0x0001)     //!< Cal version: v00.01

#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0800F800) /* Base address of Page 31, 2 Kbytes */
#define PENSEL_CAL_START_ADDRESS (ADDR_FLASH_PAGE_31)
#define PENSEL_CAL_END_ADDRESS (PENSEL_CAL_START_ADDRESS + sizeof(pensel_cal_t))


//! Calibration structure
typedef struct __attribute__((packed)) {
    uint32_t header;   //!< Header to signify start of cal region (`PENSEL_CAL_HEADER`)
    uint16_t version;  //!< Version of this cal structure (`PENSEL_CAL_VERSION`)
    float accel_offsets[3];  //!< Static offsets to be applied to accel data
    float accel_gains[3];    //!< gain multipliers to be applied to accel data
    float mag_offsets[3];    //!< Static offsets to be applied to mag data
    float mag_gains[3];      //!< gain multipliers to be applied to mag data
    uint8_t padding[1];      //!< zero padding to make flashing easier
    uint8_t checksum;        //!< Checksum to validate this memory isn't corrupt. Should checksum to zero
} pensel_cal_t;


//! Global calibration to be read by modules that need it
pensel_cal_t gCal;


// Functions to load and check the validity of calibration
ret_t cal_loadFromFlash(void);
ret_t cal_checkValidity(void);
ret_t cal_loadDefaults(void);

// Report functions for reading and writing calibration
ret_t rpt_cal_read(uint8_t * in_p, uint8_t in_len, uint8_t * out_p, uint8_t * out_len_ptr);
ret_t rpt_cal_write(uint8_t * in_p, uint8_t in_len, uint8_t * out_p, uint8_t * out_len_ptr);
