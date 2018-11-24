#include "unity.h"
#include <stdio.h>
#include "modules/utilities/FIR.h"
// TODO: make my own seperate coefficients for testing?
#include "modules/orientation/FIR_coefficients.h"

#define QUEUE_SIZE (20)

extern float sine[1000];

FIR_admin_t FIR_admin;


void test_impulseResponse(void)
{
    uint8_t i;
    float out_val, in_val;

    FIR_init(&FIR_admin, FIR_ACCEL_GRAVITY_ORDER, accel_coefficients_LPF);

    // do some output if we've defined verbose output
    #ifdef VERBOSE_OUTPUT
    printf("\nFunction: %s\n", __func__);
    printf("\n");
    #endif

    // crank an impulse function through
    for (i=0; i<FIR_ACCEL_GRAVITY_ORDER; i++) {
        if (i == 0) {
            in_val = 1;
        } else {
            in_val = 0;
        }
        out_val = FIR_run(&FIR_admin, in_val);
        #ifdef VERBOSE_OUTPUT
            printf("\titter %d: put %f in, got %f out\n", i, in_val, out_val);
            printf("\t%f == %f?\n", out_val, accel_coefficients_LPF[i]);
        #endif
        TEST_ASSERT_EQUAL_FLOAT(out_val, accel_coefficients_LPF[i]);
    }
}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_impulseResponse);

    return UNITY_END();
}
