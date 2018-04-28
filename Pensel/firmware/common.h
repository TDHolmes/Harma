/*!
 * @file    common.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   A common set of return codes and functions all files have access to.
 *
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Common macros */
#ifdef __GNUC__
    #define UNUSED_PARAM(x) UNUSED_ ## x __attribute__((__unused__))
#else
    #define UNUSED_PARAM(x) UNUSED_ ## x
#endif


#define PENSEL_VERSION_MAJOR (0)
#define PENSEL_VERSION_MINOR (6)
#define PENSEL_VERSION_GITTAG (GIT_TAG_SHORT)
// Defined by compiler: GIT_TAG_SHORT  (value of `git rev-parse --short HEAD`)

#define LSM303DLHC_ENABLE_RACCEL_STREAM (0x01)
#define LSM303DLHC_ENABLE_RMAG_STREAM (0x02)
#define LSM303DLHC_ENABLE_FACCEL_STREAM (0x04)
#define LSM303DLHC_ENABLE_FMAG_STREAM (0x08)

#define RACCEL_STREAM_REPORT_ID (0x01)
#define RMAG_STREAM_REPORT_ID (0x02)
#define FACCEL_STREAM_REPORT_ID (0x03)
#define FMAG_STREAM_REPORT_ID (0x04)

#if !defined(max)
    #define max(v1, v2) (v1 >= v2 ? v1 : v2)
#endif

#if !defined(min)
    #define min(v1, v2) (v1 < v2 ? v1 : v2)
#endif

#if !defined(NULL)
    #define NULL ((void *) 0)
#endif


//! Common return type for the entire project
typedef enum {
    RET_OK,         //!< The return code if all goes well
    RET_VAL_ERR,    //!< If the values given cause an error
    RET_NODATA_ERR, //!< If not enough data was given
    RET_NOMEM_ERR,  //!< If not enough memory is available
    RET_LEN_ERR,    //!< If there was some sort of length related error
    RET_COM_ERR,    //!< Some sort of communication error
    RET_BUSY_ERR,   //!< Some sort of busy resource / timeout
    RET_GEN_ERR,    //!< Lame catch all general error
    RET_NORPT_ERR,  //!< No available report
    RET_INVALID_ARGS_ERR,  //!< Incorrect arguments to the function called
    RET_MAX_LEN_ERR,       //!< Maximum length was violated
    RET_WDG_SET,           //!< Return code if the watchdog flag was set on reset
    RET_CAL_ERR,           //!< General error relating to calibration
    RET_BAD_CHECKSUM,      //!< General checksum related error (didn't match)
} ret_t;

//! Structure to keep track of our critical errors
typedef struct __attribute__((packed)) {
    #ifdef WATCHDOG_ENABLE
        uint32_t wdg_reset : 1;
    #endif
    uint32_t cal_header_err : 1;
    uint32_t cal_version_err : 1;
    uint32_t cal_checksum_err : 1;
    uint32_t UART_droppedBytes;
    uint32_t UART_dequeuedBytes;
    uint32_t UART_queuedBytes;
} critical_errors_t;


void fatal_error_handler(char file[], uint32_t line, int8_t err_code);
