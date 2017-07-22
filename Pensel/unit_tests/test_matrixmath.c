#include "unity.h"
#include <stdio.h>
#include "matrixmath.h"


void test_unitVectorCross(void) {
    // setup some variables...
    ret_t retval;
    matrix_1x3_t x_unit;
    matrix_1x3_t y_unit;
    matrix_1x3_t m_result;
    x_unit.matrix[0][0] = 1;
    x_unit.matrix[0][1] = 0;
    x_unit.matrix[0][2] = 0;
    y_unit.matrix[0][0] = 0;
    y_unit.matrix[0][1] = 1;
    y_unit.matrix[0][2] = 0;

    // Run the function
    #ifdef VERBOSE_OUTPUT
        printf("\nFunction: %s\n", __func__);
        printf("\tEntering matrix_cross...\n");
    #endif
    retval = matrix_cross(x_unit, y_unit, &m_result);

    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
        printf("\tresulting vector: ");
        printf("<%f, %f, %f>\n", m_result.matrix[0][0], m_result.matrix[0][1],
                                 m_result.matrix[0][2]);
        printf("\tretval: %d\n", retval);
        printf("\n");
    #endif

    // Check pass / failure
    TEST_ASSERT_EQUAL_HEX8(RET_OK, retval);
    TEST_ASSERT_EQUAL_FLOAT(0, m_result.matrix[0][0]);
    TEST_ASSERT_EQUAL_FLOAT(0, m_result.matrix[0][1]);
    TEST_ASSERT_EQUAL_FLOAT(1, m_result.matrix[0][2]);
}


int main(void)
{
    UNITY_BEGIN();
    #ifdef VERBOSE_OUTPUT
        printf("Starting %s tests\n", __FILE__);
    #endif
    RUN_TEST(test_unitVectorCross);

    return UNITY_END();
}
