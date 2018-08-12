/*!
 * @file    main.c
 * @author  Tyler Holmes
 *
 * @date    20-May-2017
 * @brief   Main project logic.
 */

#include <stdint.h>
#include <string.h>
#include "common.h"

// Sensors
#include "peripherals/hardware/hardware.h"   // HW support and button / switch functions

// Communications and such
#include "peripherals/I2C/I2C.h"
#include "peripherals/UART/UART.h"

// Algssss
#include "modules/orientation/orientation.h"
#include "modules/calibration/cal.h"

// STM Drivers
#include "peripherals/stm32f3/stm32f3xx_hal_def.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"
#include "peripherals/stm32f3-configuration/stm32f3xx_hal_conf.h"


//! HAL millisecond tick
extern __IO uint32_t uwTick;
// Global variables to influence state

critical_errors_t gCriticalErrors;


static inline void check_retval_fatal(char * filename, uint32_t lineno, ret_t retval) {
    if (retval != RET_OK) {
        fatal_error_handler(filename, lineno, (int8_t)retval);
    }
}

void clear_critical_errors(void) {
    #ifdef WATCHDOG_ENABLE
        gCriticalErrors.wdg_reset = 0;
    #endif
}


/*! Main function code. Does the following:
 *      1. Initializes all sub-modules
 *      2. Loops forever and behaves as such given switch state:
 *          Switch 0: Runs report parsing only
 *          Switch 1: Runs debug print output only
 */
int main(void)
{
    // ret_t retval;
    uint32_t subcount = 0;

    // system configuration...
    HAL_Init();
    SystemClock_Config();
    configure_pins();
    clear_critical_errors();

    LED_set(LED_0, 0);
    LED_set(LED_1, 0);

    while (true) {

        // let the user know roughly how many workloops are ocurring
        if (subcount == 100000) {
            LED_toggle(LED_1);
            subcount = 0;
        } else {
            subcount++;
        }

    } /* while (true) */
} /* main() */


/*! millisecond ISR that increments the global time as well as calls some functions
 *  that need to be periodically serviced. We do a few things:
 *
 *     1. Take care of button and switch debouncing every 10 ms
 *        - If we've changed states in interrupt context and enough time has elapsed (``)
 *          we will change the button / switch state
 *     2. Pet the watchdog every 5 ms. Must be pet within 10 ms!
 *     3. Toggle the heartbeat LED every 1 second
 */
void HAL_IncTick(void)
{
    // sub counters
    static uint16_t second_count = 0;

    // increment the ms timer
    uwTick++;

    // Heartbeat LED flash
    if (second_count >= 1000) {
        second_count = 0;
        LED_toggle(LED_0);
    } else {
        second_count++;
    }
}


/*! Error handler that is called when fatal exceptions are found.
 *
 * @param file (char *): File in which the error comes from. Use the __FILE__ macro.
 * @param line (uint32_t): Line number in which the error comes from. Use the __LINE__ macro.
 * @param err_code (int8_t): Error code that was thrown. Usually the ret_t value
    that caused the fail.
 */
void fatal_error_handler(char file[], uint32_t line, int8_t err_code)
{
    // FREAK OUT
    #ifdef DEBUG
        uint32_t timer_count, i = 0;
        LED_set(LED_0, 0);
        LED_set(LED_1, 1);

        // Can't rely on HAL tick as we maybe in the ISR context...
        while (1) {
            UART_sendString(file);
            UART_sendint((int64_t)line);
            UART_sendint((int64_t)err_code);
            UART_sendString("\r\n");

            for (i = 0; i < 25; i++) {
                while (timer_count < 100000) {
                    timer_count++;
                }
                timer_count = 0;
            }
            LED_toggle(LED_0);
            LED_toggle(LED_1);
        }
    #else
        // TODO: Reset everything
        // TODO: Log failure
        // for now, just let the watchdog happen
        while (1);
    #endif
}

// HAL uses this function. Call our error function.
void assert_failed(uint8_t* file, uint32_t line)
{
    fatal_error_handler((char *)file, line, -1);
}
