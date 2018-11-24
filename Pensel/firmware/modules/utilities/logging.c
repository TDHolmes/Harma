/*
 *
 */
#include "logging.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

typedef struct log_admin {
    log_level_t log_level;
    ret_t (*write_func)(char *);
    uint32_t (*get_curTime_ms)(void);
} log_admin_t;

static log_admin_t gLogAdmin;
static char gMessageBuffer[LOGGING_MAX_MSG_LEN];


int populateLogLevel(log_level_t level, char buffer_ptr[]);


/*! Initializes the logging module with some infoooo
 */
ret_t log_init(log_level_t level, ret_t (*write_func_ptr)(char *),
               uint32_t (*get_curTime_ms_ptr)(void))
{
    gLogAdmin.log_level = level;
    gLogAdmin.write_func = write_func_ptr;
    gLogAdmin.get_curTime_ms = get_curTime_ms_ptr;

    return RET_OK;
}


/*! Log a log message at the given level and location in the code.
 *
 */
ret_t log_logMessage(log_level_t level, const char filename[], const char funcname[],
                     uint32_t linenum, const char msg_ptr[])
{
    int logLevelBytes;
    int finalLogBytes;
    if (level > gLogAdmin.log_level) {
        // Message more verbose than our set level. Don't do anything
        return RET_OK;
    }

    logLevelBytes = populateLogLevel(level, gMessageBuffer);
    if (logLevelBytes <= 0) {
        return RET_NOMEM_ERR;
    }
    finalLogBytes = snprintf(gMessageBuffer + logLevelBytes, sizeof(gMessageBuffer) - logLevelBytes,
                             "%s:%u (in %s): %s\n", filename, linenum, funcname, msg_ptr);

    if (finalLogBytes <= 0 || finalLogBytes + logLevelBytes >= (int)sizeof(gMessageBuffer)) {
        return RET_NOMEM_ERR;
    }

    return gLogAdmin.write_func(gMessageBuffer);
}

void log_changeLevel(log_level_t newLevel) {
    gLogAdmin.log_level = newLevel;
}

/* --- Private Helper Functions --- */

/*! Given a log level and string buffer to write into, write in the log level
 *    and current time. Return number of bytes this takes up.
 */
int populateLogLevel(log_level_t level, char buffer_ptr[])
{
    char logLevelMsg[16];
    uint32_t curTime_ms = gLogAdmin.get_curTime_ms();

    switch (level) {
        case kLogLevelError:
            strncpy(logLevelMsg, "ERROR ", sizeof(logLevelMsg));
            break;

        case kLogLevelWarning:
            strncpy(logLevelMsg, "WARNING ", sizeof(logLevelMsg));
            break;

        case kLogLevelInfo:
            strncpy(logLevelMsg, "INFO ", sizeof(logLevelMsg));
            break;

        case kLogLevelDebug:
            strncpy(logLevelMsg, "DEBUG ", sizeof(logLevelMsg));
            break;
    }

    return sprintf(buffer_ptr, "%08ums - %s ", curTime_ms, logLevelMsg);
}