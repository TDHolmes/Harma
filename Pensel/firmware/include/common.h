/*!
 * @file    common.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   A common set of return codes and functions all files have access to.
 *
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

/* Common macros */
#ifdef __GNUC__
    #define UNUSED_PARAM(x) UNUSED_ ## x __attribute__((__unused__))
#else
    #define UNUSED_PARAM(x) UNUSED_ ## x
#endif


//! Common return type for the entire project
typedef enum {
    RET_OK,         //!< The return code if all goes well
    RET_VAL_ERR,    //!< If the values given cause an error
    RET_NODATA_ERR, //!< If not enough data was given
    RET_NOMEM_ERR,  //!< If not enough memory is available
    RET_LEN_ERR,    //!< If there was some sort of length related error
    RET_COM_ERR,    //!< Some sort of communication error
    RET_BUSY_ERR,   //!< Some sort of busy resource
    RET_GEN_ERR,    //!< Lame catch all general error
    RET_NORPT_ERR,  //!< No available report
    RET_INVALID_ARGS_ERR,  //!< Incorrect arguments to the function called
    RET_MAX_LEN_ERR,       //!< Maximum length was violated
    RET_WDG_SET            //!< Return code if the watchdog flag was set on reset
} ret_t;


void fatal_error_handler(char file[], uint32_t line, int8_t err_code);


#endif /* _COMMON_H_ */
