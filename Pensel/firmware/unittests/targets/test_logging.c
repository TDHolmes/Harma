#include "unity.h"
#include "common.h"
// #include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "modules/utilities/logging.h"

#define MAX_MSG_LEN (LOGGING_MAX_MSG_LEN + 2)

uint32_t current_time_ms;       //!< The time we pretend it is
bool write_func_called;         //!< we set this to True if logging calls our write function

/*! the string we check against. Populate this with the correct
 *    string before running a given log test */
char expected_message[MAX_MSG_LEN];
char actual_message[MAX_MSG_LEN];

ret_t recieve_and_check_string(char *str) {
    write_func_called = true;
    strncpy(actual_message, str, sizeof(actual_message));
    return RET_OK;
}

void clear_flags(void) {
    write_func_called = false;
}

bool write_func_was_called(void) {
    bool was_called = write_func_called;
    write_func_called = false;
    return was_called;
}

uint32_t get_curtime_ms(void) {
    return current_time_ms;
}

void configure_logging(log_level_t logLevel) {
    ret_t ret;
    // clear previous time/string/flags
    clear_flags();
    current_time_ms = 0;
    memset(expected_message, 0, sizeof(expected_message));

    // init the module!
    ret = log_init(logLevel, recieve_and_check_string, get_curtime_ms);
    TEST_ASSERT_EQUAL(RET_OK, ret);
}

void test_basicError(void)
{
    char filename[] = "testFilename";
    char funcname[] = "testFuncname";
    char message[] = "hello world";
    int linenum = 42;

    configure_logging(kLogLevelError);

    sprintf(expected_message, "00000000ms - ERROR \t%s:%u (in %s): %s\n", filename, linenum, funcname, message);
    log_logMessage(kLogLevelError, filename, funcname, linenum, message);
    TEST_ASSERT_EQUAL_STRING(expected_message, actual_message);
    TEST_ASSERT_TRUE( write_func_was_called() );
}

void test_basicWarning(void)
{
    char filename[] = "testFilename";
    char funcname[] = "testFuncname";
    char message[] = "hello world";
    int linenum = 42;

    configure_logging(kLogLevelWarning);

    sprintf(expected_message, "00000000ms - WARNING \t%s:%u (in %s): %s\n", filename, linenum,
            funcname, message);
    log_logMessage(kLogLevelWarning, filename, funcname, linenum, message);
    TEST_ASSERT_EQUAL_STRING(expected_message, actual_message);
    TEST_ASSERT_TRUE( write_func_was_called() );
}

void test_basicInfo(void)
{
    char filename[] = "testFilename";
    char funcname[] = "testFuncname";
    char message[] = "hello world";
    int linenum = 42;

    configure_logging(kLogLevelInfo);

    sprintf(expected_message, "00000000ms - INFO \t%s:%u (in %s): %s\n", filename, linenum,
            funcname, message);
    log_logMessage(kLogLevelInfo, filename, funcname, linenum, message);
    TEST_ASSERT_EQUAL_STRING(expected_message, actual_message);
    TEST_ASSERT_TRUE( write_func_was_called() );
}

void test_basicDebug(void)
{
    char filename[] = "testFilename";
    char funcname[] = "testFuncname";
    char message[] = "hello world";
    int linenum = 42;

    configure_logging(kLogLevelDebug);

    sprintf(expected_message, "00000000ms - DEBUG \t%s:%u (in %s): %s\n", filename, linenum, funcname,
            message);
    log_logMessage(kLogLevelDebug, filename, funcname, linenum, message);
    TEST_ASSERT_EQUAL_STRING(expected_message, actual_message);
    TEST_ASSERT_TRUE( write_func_was_called() );
}

void test_logsAtSameLevel(void)
{
    // make sure when configured to a given level, that level message and above make it through

    configure_logging(kLogLevelError);
    // These shouldn't write to the log
    LOG_MSG(kLogLevelDebug,   "testing 123"); TEST_ASSERT_FALSE(write_func_was_called() );
    LOG_MSG(kLogLevelInfo,    "testing 123"); TEST_ASSERT_FALSE(write_func_was_called() );
    LOG_MSG(kLogLevelWarning, "testing 123"); TEST_ASSERT_FALSE(write_func_was_called() );
    // These should
    LOG_MSG(kLogLevelError,   "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );

    configure_logging(kLogLevelWarning);
    // These shouldn't write to the log
    LOG_MSG(kLogLevelDebug,   "testing 123"); TEST_ASSERT_FALSE(write_func_was_called() );
    LOG_MSG(kLogLevelInfo,    "testing 123"); TEST_ASSERT_FALSE(write_func_was_called() );
    // These should
    LOG_MSG(kLogLevelWarning, "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );
    LOG_MSG(kLogLevelError,   "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );

    configure_logging(kLogLevelInfo);
    // These shouldn't write to the log
    LOG_MSG(kLogLevelDebug,   "testing 123"); TEST_ASSERT_FALSE(write_func_was_called() );
    // These should
    LOG_MSG(kLogLevelInfo,    "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );
    LOG_MSG(kLogLevelWarning, "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );
    LOG_MSG(kLogLevelError,   "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );

    configure_logging(kLogLevelDebug);
    // These should
    LOG_MSG(kLogLevelDebug,   "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );
    LOG_MSG(kLogLevelInfo,    "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );
    LOG_MSG(kLogLevelWarning, "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );
    LOG_MSG(kLogLevelError,   "testing 123"); TEST_ASSERT_TRUE(write_func_was_called() );
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_basicError);
    RUN_TEST(test_basicWarning);
    RUN_TEST(test_basicInfo);
    RUN_TEST(test_basicDebug);
    RUN_TEST(test_logsAtSameLevel);

    return UNITY_END();
}
