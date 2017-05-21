#include "unity.h"
#include <stdio.h>
#include "newqueue.h"

#define QUEUE_SIZE (20)
newqueue_t queue;


void queue_init(uint8_t num_elements, uint8_t val_size)
{
    newqueue_init(&queue, num_elements, val_size);
}


void test_basicSetGet(void)
{
    uint8_t val;
    queue_init(QUEUE_SIZE, 1);
    val = 42;
    newqueue_push(&queue, (void *)&val);
    newqueue_pop(&queue, (void *)&val, false);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\tPut %d onto buffer\n", 42);
        printf("\tGot %d back\n", val);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(42, val);
    newqueue_deinit(&queue);
}


void test_basicSetIncrementsUnreadCount(void)
{
    uint8_t val;
    queue_init(QUEUE_SIZE, 1);
    val = 42;
    newqueue_push(&queue, (void *)&val);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\tPut %d onto buffer\n", 42);
        printf("\t%d unread values\n", queue.unread_items);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(1, queue.unread_items);
    newqueue_deinit(&queue);
}


void test_basicGetDecrementsUnreadCount(void)
{
    uint8_t val;
    queue_init(QUEUE_SIZE, 1);

    // push some vals onto the queue
    val = 42; newqueue_push(&queue, (void *)&val);
    val = 24; newqueue_push(&queue, (void *)&val);
    val = 84; newqueue_push(&queue, (void *)&val);

    // pop one off
    newqueue_pop(&queue, (void *)&val, false);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\tPut 3 values onto buffer and popped off 1\n");
        printf("\t%d unread values\n", queue.unread_items);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(2, queue.unread_items);
    newqueue_deinit(&queue);
}


void test_overwriteCountIncreases(void)
{
    queue_init(QUEUE_SIZE, 1);
    for (uint8_t i = 0; i <= QUEUE_SIZE; i++) {
        newqueue_push(&queue, (void *)&i);
    }
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\t%d + 1 values onto buffer of size %d\n", QUEUE_SIZE, QUEUE_SIZE);
        printf("\t%d overwritten values\n", queue.overwrite_count);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(1, queue.overwrite_count);
    newqueue_deinit(&queue);
}


void test_overwrite_getExpectedValue(void)
{
    uint8_t val;
    queue_init(QUEUE_SIZE, 1);
    for (uint8_t i = 0; i <= QUEUE_SIZE; i++) {
        newqueue_push(&queue, (void *)&i);
    }
    newqueue_pop(&queue, (void *)&val, false);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\t%d + 1 values onto buffer of size %d\n", QUEUE_SIZE, QUEUE_SIZE);
        printf("\tgot 1 value back == %d\n", val);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(1, val);
    newqueue_deinit(&queue);
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
