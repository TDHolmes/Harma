/*
 *
 */
#include <stdint.h>

#include "common.h"


typedef enum {
    kLogLevelError,
    kLogLevelWarning,
    kLogLevelInfo,
    kLogLevelDebug
} log_level_t;


#define LOG_MSG(__LVL__, __MSG__)  log_logMessage(__LVL__, __FILE__, __func__, __LINE__, __MSG__)

#define LOG_MSG_FMT(__LVL__, __MSG__, ...)  char msg_buff[64]; \
        sprintf(msg_buff, __MSG__, __VA_ARGS__); \
        LOG_MSG(__LVL__, msg_buff) \


ret_t log_init(
    log_level_t level,
    ret_t (*write_func_ptr)(char *),
    uint32_t (*get_cur_time_ptr)(void));

ret_t log_logMessage(
    log_level_t level,
    const char filename[],
    const char funcname[],
    uint32_t linenum,
    const char msg_ptr[]);
