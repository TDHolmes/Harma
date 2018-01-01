#include "unity.h"
#include <stdio.h>
#include "newqueue.h"

#define QUEUE_SIZE (20)
#define LARGE_NUMBER (999999)

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
    newqueue_push(&queue, (void *)&val, 1);
    val = 0;
    newqueue_pop(&queue, (void *)&val, 1, false);
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


void test_basicSetGet_peak(void)
{
    uint8_t val;
    queue_init(QUEUE_SIZE, 1);
    val = 42;
    newqueue_push(&queue, (void *)&val, 1);
    val = 0;
    newqueue_pop(&queue, (void *)&val, 1, true);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\tPut %d onto buffer\n", 42);
        printf("\tGot %d back (peak)\n", val);
        printf("\t%d unread items\n", queue.unread_items);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(42, val);
    TEST_ASSERT_EQUAL_HEX8(1, queue.unread_items);
    newqueue_deinit(&queue);
}


void test_basicSetIncrementsUnreadCount(void)
{
    uint8_t val;
    queue_init(QUEUE_SIZE, 1);
    val = 42;
    newqueue_push(&queue, (void *)&val, 1);
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
    val = 42; newqueue_push(&queue, (void *)&val, 1);
    val = 24; newqueue_push(&queue, (void *)&val, 1);
    val = 84; newqueue_push(&queue, (void *)&val, 1);

    // pop one off
    newqueue_pop(&queue, (void *)&val, 1, false);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\tPut 3 values onto buffer and popped off 1\n");
        printf("\t%d unread values\n", queue.unread_items);
        printf("\tPushed %d on first, got %d back\n",42, val);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(2, queue.unread_items);
    TEST_ASSERT_EQUAL_HEX8(42, val);
    newqueue_deinit(&queue);
}


void test_multipleGetDecrementsUnreadCount(void)
{
    uint8_t vals[2];
    queue_init(QUEUE_SIZE, 1);

    // push some vals onto the queue
    vals[0] = 42; newqueue_push(&queue, (void *)&vals[0], 1);
    vals[0] = 24; vals[1] = 84;
    newqueue_push(&queue, (void *)vals, 2);

    // pop one off
    vals[0] = 0; vals[1] = 0;
    newqueue_pop(&queue, (void *)&vals, 2, false);
    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\tPut 3 values onto buffer and popped off 2\n");
        printf("\t%d unread values\n", queue.unread_items);
        printf("\tPushed %d on first, got %d back\n",42, vals[0]);
        printf("\tPushed %d on second, got %d back\n",24, vals[1]);
        printf("\n");
    #endif

    TEST_ASSERT_EQUAL_HEX8(1, queue.unread_items);
    TEST_ASSERT_EQUAL_HEX8(42, vals[0]);
    TEST_ASSERT_EQUAL_HEX8(24, vals[1]);
    newqueue_deinit(&queue);
}


void test_fullQueue_pushPop(void)
{
    uint8_t val;
    queue_init(QUEUE_SIZE, 1);
    for (uint8_t i = 0; i < QUEUE_SIZE; i++) {
        newqueue_push(&queue, (void *)&i, 1);
    }

    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
    #endif

    for (uint8_t i = 0; i < QUEUE_SIZE; i++) {
        newqueue_pop(&queue, (void *)&val, 1, false);
        #ifdef VERBOSE_OUTPUT
            printf("\t%d == %d?\n", i, val);
        #endif
        TEST_ASSERT_EQUAL_HEX8(i, val);
    }

    newqueue_deinit(&queue);
}


void test_overwriteCountIncreases(void)
{
    queue_init(QUEUE_SIZE, 1);
    for (uint8_t i = 0; i <= QUEUE_SIZE; i++) {
        newqueue_push(&queue, (void *)&i, 1);
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
        newqueue_push(&queue, (void *)&i, 1);
    }
    newqueue_pop(&queue, (void *)&val, 1, false);
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


/************* Large number tests! *****************/


void test_fullQueue_pushPop_int32(void)
{
    int32_t val;
    queue_init(QUEUE_SIZE, 4);

    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
    #endif

    // build up queue
    for (int32_t i = 0; i < QUEUE_SIZE; i++) {
        val = LARGE_NUMBER + i;
        // #ifdef VERBOSE_OUTPUT
        //     printf("\tPushing %ld on\n", val);
        // #endif
        newqueue_push(&queue, (void *)&val, 1);
    }

    // Pop it out and check
    for (uint8_t i = 0; i < QUEUE_SIZE; i++) {
        newqueue_pop(&queue, (void *)&val, 1, false);
        #ifdef VERBOSE_OUTPUT
            printf("\t%ld == %ld?\n", LARGE_NUMBER + i, val);
        #endif
        TEST_ASSERT_EQUAL_HEX32(LARGE_NUMBER + i, val);
    }

    newqueue_deinit(&queue);
}


void test_fullQueue_pushPop_int32neg(void)
{
    int32_t val;
    queue_init(QUEUE_SIZE, 4);

    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
    #endif

    // build up queue
    for (int32_t i = 0; i < QUEUE_SIZE; i++) {
        val = i - LARGE_NUMBER;
        // #ifdef VERBOSE_OUTPUT
        //     printf("\tPushing %i on\n", val);
        // #endif
        newqueue_push(&queue, (void *)&val, 1);
    }

    // Pop it out and check
    for (int32_t i = 0; i < QUEUE_SIZE; i++) {
        newqueue_pop(&queue, (void *)&val, 1, false);
        #ifdef VERBOSE_OUTPUT
            printf("\t%i == %i?\n", (-LARGE_NUMBER) + i, val);
        #endif
        TEST_ASSERT_EQUAL((-LARGE_NUMBER) + i, val);
    }

    newqueue_deinit(&queue);
}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_basicSetGet);
    RUN_TEST(test_basicSetGet_peak);
    RUN_TEST(test_fullQueue_pushPop);

    RUN_TEST(test_basicSetIncrementsUnreadCount);
    RUN_TEST(test_basicGetDecrementsUnreadCount);
    RUN_TEST(test_multipleGetDecrementsUnreadCount);

    RUN_TEST(test_overwriteCountIncreases);
    RUN_TEST(test_overwrite_getExpectedValue);

    RUN_TEST(test_fullQueue_pushPop_int32);
    RUN_TEST(test_fullQueue_pushPop_int32neg);

    return UNITY_END();
}
