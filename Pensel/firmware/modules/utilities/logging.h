/*
 *
 */
#include <stdint.h>
#include <stdio.h>
#include "common.h"

#define LOGGING_MAX_MSG_LEN (512)

typedef enum {
    kLogLevelError,   //!< Error level (catastrophic failure)
    kLogLevelWarning, //!< Warning level (not catastrophic failure)
    kLogLevelInfo,    //!< Info level (Letting you know high level what's happening)
    kLogLevelDebug    //!< Debug level (verbose debugging information)
} log_level_t;

#define LOG_MSG(__LVL__, __MSG__) log_logMessage(__LVL__, __BASE_FILE__, __func__, __LINE__, __MSG__)

#define LOG_MSG_FMT(__LVL__, __MSG__, ...)                                \
    char __log_msg_fmt_buff[LOGGING_MAX_MSG_LEN];                         \
    snprintf(__log_msg_fmt_buff, sizeof(__log_msg_fmt_buff), __MSG__, __VA_ARGS__); \
    LOG_MSG(__LVL__, __log_msg_fmt_buff)

// TODO: document!
/*!
 *
 */
ret_t log_init(log_level_t level, ret_t (*write_func_ptr)(char *),
               uint32_t (*get_cur_time_ptr)(void));

/*!
 *
 */
void log_changeLevel(log_level_t newLevel);

/*!
 *
 */
ret_t log_logMessage(log_level_t level, const char filename[], const char funcname[],
                     uint32_t linenum, const char msg_ptr[]);
