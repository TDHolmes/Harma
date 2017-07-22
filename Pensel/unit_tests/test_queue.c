#include "unity.h"
#include <stdio.h>
#include "../firmware/include/queue.h"

#define QUEUE_SIZE (20)

uint8_t queue_buffer[QUEUE_SIZE];
queue_t queue_admin;


void test_basicSetGet(void)
{
    uint8_t val;
    queue_init(&queue_admin);
    queue_buffer[queue_admin.head_ind] = 42;
    queue_increment_head(&queue_admin, QUEUE_SIZE);
    val = queue_buffer[queue_admin.tail_ind];
    queue_increment_tail(&queue_admin, QUEUE_SIZE);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
    printf("\nFunction: %s\n", __func__);
    printf("\tPut %d onto buffer\n", 42);
    printf("\tGot %d back\n", val);
    printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(42, val);
}


void test_basicSetIncrementsUnreadCount(void)
{
    queue_init(&queue_admin);
    queue_buffer[queue_admin.head_ind] = 42;
    queue_increment_head(&queue_admin, QUEUE_SIZE);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
    printf("\nFunction: %s\n", __func__);
    printf("\tPut %d onto buffer\n", 42);
    printf("\t%d unread values\n", queue_admin.unread_items);
    printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(1, queue_admin.unread_items);
}


void test_basicGetDecrementsUnreadCount(void)
{
    uint8_t val;
    queue_init(&queue_admin);
    queue_buffer[queue_admin.head_ind] = 42;
    queue_increment_head(&queue_admin, QUEUE_SIZE);
    queue_buffer[queue_admin.head_ind] = 24;
    queue_increment_head(&queue_admin, QUEUE_SIZE);
    queue_buffer[queue_admin.head_ind] = 84;
    queue_increment_head(&queue_admin, QUEUE_SIZE);
    val = queue_buffer[queue_admin.tail_ind];
    queue_increment_tail(&queue_admin, QUEUE_SIZE);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
    printf("\nFunction: %s\n", __func__);
    printf("\tPut 3 values onto buffer and popped off 1\n");
    printf("\t%d unread values\n", queue_admin.unread_items);
    printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(2, queue_admin.unread_items);
}



void test_overwriteCountIncreases(void)
{
    queue_init(&queue_admin);
    for (uint8_t i = 0; i <= QUEUE_SIZE; i++) {
        queue_buffer[queue_admin.head_ind] = i;
        queue_increment_head(&queue_admin, QUEUE_SIZE);
    }
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
    printf("\nFunction: %s\n", __func__);
    printf("\t%d + 1 values onto buffer of size %d\n", QUEUE_SIZE, QUEUE_SIZE);
    printf("\t%d overwritten values\n", queue_admin.overwrite_count);
    printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(1, queue_admin.overwrite_count);
}


void test_overwrite_getExpectedValue(void)
{
    uint8_t val;
    queue_init(&queue_admin);
    for (uint8_t i = 0; i <= QUEUE_SIZE; i++) {
        queue_buffer[queue_admin.head_ind] = i;
        queue_increment_head(&queue_admin, QUEUE_SIZE);
    }
    val = queue_buffer[queue_admin.tail_ind];
    queue_increment_tail(&queue_admin, QUEUE_SIZE);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
    printf("\nFunction: %s\n", __func__);
    printf("\t%d + 1 values onto buffer of size %d\n", QUEUE_SIZE, QUEUE_SIZE);
    printf("\tgot 1 value back == %d\n", val);
    printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(1, val);
}



int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_basicSetGet);
    RUN_TEST(test_basicSetIncrementsUnreadCount);
    RUN_TEST(test_basicGetDecrementsUnreadCount);
    RUN_TEST(test_overwriteCountIncreases);
    RUN_TEST(test_overwrite_getExpectedValue);

    return UNITY_END();
}
