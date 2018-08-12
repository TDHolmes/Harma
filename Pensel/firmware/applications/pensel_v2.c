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

// Algs and utilities
#include "modules/utilities/scheduler.h"
#include "modules/utilities/logging.h"
#include "modules/LSM9DS1/LSM9DS1.h"

// STM Drivers
#include "peripherals/stm32f3/stm32f3xx_hal_def.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"
#include "peripherals/stm32f3-configuration/stm32f3xx_hal_conf.h"
#include "peripherals/stm32f3/stm32f3xx_hal_pcd.h"

// USB
#include "peripherals/stm32-usb/usb_lib.h"
#include "peripherals/USB/hw_config.h"
#include "peripherals/USB/usb_desc.h"
#include "peripherals/USB/usb_pwr.h"

#include "modules/CDC/cdc.h"

//! Level of log messages we will print out to the user
#define LOGGING_LEVEL   (kLogLevelInfo)


//! HAL millisecond tick
extern __IO uint32_t uwTick;
// Global variables to influence state

//! The critical errors for the entire system
critical_errors_t gCriticalErrors;
//! The scheduler for all tasks to be run in the main context
schedule_t gMainSchedule;

// Local functions
ret_t heartbeat(int32_t * new_callback_time_ms);
ret_t workloop_flash(int32_t * new_callback_time_ms);
ret_t button_handler(int32_t * new_callback_time_ms);
void USB_pullup_set(uint8_t value);


#ifdef WATCHDOG_ENABLE
    //! Global indicating whether or not to pet the watchdog
    static bool gPetWdg = false;

    void wdg_captureAlert(void)
    {
        // If we want, we can catch if a watchdog has ocurred
        uint32_t subcount = 0;

        LED_set(LED_0, 0);
        LED_set(LED_1, 1);
        while (1) {
            subcount++;
            if (subcount > 1000000) {
                subcount = 0;
                LED_toggle(LED_0);
                LED_toggle(LED_1);
            }
        }
    }

    ret_t watchdog_pet(int32_t * new_callback_time_ms)
    {
        *new_callback_time_ms = 500;
        UART_sendString("w");
        // every 500 ms, pet the watchdog. Need to pet the watchdog every ~1 s!
        #ifdef WATCHDOG_ENABLE
            if (gPetWdg) {
                wdg_pet();
            }
        #endif
        return RET_OK;
    }
#endif

static inline void check_retval_fatal(char * filename, uint32_t lineno, ret_t retval)
{
    if (retval != RET_OK) {
        fatal_error_handler(filename, lineno, (int8_t)retval);
    }
}


void clear_critical_errors(void)
{
    #ifdef WATCHDOG_ENABLE
        gCriticalErrors.wdg_reset = 0;
    #endif
}


ret_t print(char * string)
{
    uint32_t string_len = 0;
    if (bDeviceState != CONFIGURED) {
        return RET_BUSY_ERR;
    }

    // calculate length of string to check if it's too long
    for (char * c = string; ; c += 1) {
        if (*c == 0) {
            break;
        }
        string_len += 1;
    }

    return cdc_inTransfer_start((uint8_t *)string, string_len);
}


/*! Main function code. Does the following:
 *      1. Initializes all sub-modules
 *      2. Loops forever and behaves as such given switch state:
 *          Switch 0: Runs report parsing only
 *          Switch 1: Runs debug print output only
 */
int main(void)
{
    uint8_t i = 0;
    int32_t time_until_next_cb_ms;
    gyro_ODR_t gyro_ODR = kGyroODR_14_9_Hz;
    accel_ODR_t accel_ODR = kAccelODR_10_Hz;
    gyro_fullscale_t gyro_FS = k2000DPS_fullscale;
    accel_fullscale_t accel_FS = k16g_fullscale;

    // system configuration...
    HAL_Init();
    SystemClock_Config();
    configure_pins();
    clear_critical_errors();

    // Initialize UART and logging
    check_retval_fatal(__FILE__, __LINE__, UART_init(460800) );
    UART_sendString("\n");  // Get rid of annoying crap
    log_init(LOGGING_LEVEL, UART_sendString, HAL_GetTick);
    LOG_MSG(kLogLevelInfo, "Log module initialized");

#ifdef WATCHDOG_ENABLE
    if ( wdg_isSet() ) {
        // set a report variable in critical errors
        gCriticalErrors.wdg_reset = 1;
        #ifdef WATCHDOG_CAPTURE
            wdg_captureAlert();
        #endif
    }
    // Initialize the watchdog
    check_retval_fatal(__FILE__, __LINE__, wdg_init() );
    gPetWdg = true;
#endif

    // peripheral configuration
    check_retval_fatal(__FILE__, __LINE__, I2C_init() );

    LED_set(LED_0, 0);
    LED_set(LED_1, 0);

    hw_USB_init();
    USB_init();
    check_retval_fatal(__FILE__, __LINE__,
        LSM9DS1_init(gyro_ODR, gyro_FS, accel_ODR, accel_FS) );

    // initalize the scheduler and add some periodic tasks
    scheduler_init(&gMainSchedule);
    scheduler_add(&gMainSchedule, 0, heartbeat, &i);
    scheduler_add(&gMainSchedule, 0, workloop_flash, &i);
    scheduler_add(&gMainSchedule, 0, button_handler, &i);

    #ifdef WATCHDOG_ENABLE
        scheduler_add(&gMainSchedule, 0, watchdog_pet, &i);
    #endif


    // --- Main scheduler super loop!
    uint32_t sleep_until = 0;
    while (true) {
        sleep_until = HAL_GetTick();  // grab start of loop time
        time_until_next_cb_ms = scheduler_run(&gMainSchedule, sleep_until);
        sleep_until += time_until_next_cb_ms;

        if (time_until_next_cb_ms > 0) {
            // TODO: sleep CPU instead of dumb delay
            if (HAL_GetTick() < sleep_until ) {
                HAL_Delay( sleep_until - HAL_GetTick() );
            }
        }
    } /* while (true) */
} /* main() */


/* --- Some common callbacks to be run --- */

ret_t heartbeat(int32_t * new_callback_time_ms)
{
    static uint8_t tick = 0;
    *new_callback_time_ms = 1000;
    LED_toggle(LED_0);
    TimingPin_toggle();

    uint8_t status;
    if ( LSM9DS1_readStatus(&status) == RET_OK ) {
        LOG_MSG_FMT(kLogLevelInfo, "LSM status: %i", status);
    } else {
        LOG_MSG(kLogLevelWarning, "LSM status read FAILED");
    }

    if (tick % 2 == 0) {
        LOG_MSG(kLogLevelDebug, "\tTick");
    } else if (tick % 2 == 1) {
        LOG_MSG(kLogLevelDebug, "\tTock");
    }
    tick += 1;
    return RET_OK;
}


ret_t workloop_flash(int32_t * new_callback_time_ms)
{
    static uint16_t i = 0;
    *new_callback_time_ms = 0;

    // toggle LED every 10,000 itterations over the workloop
    if (i++ == 0) {
        LED_toggle(LED_1);
    }
    return RET_OK;
}


ret_t button_handler(int32_t * new_callback_time_ms)
{
    // UART_sendString("b");
    *new_callback_time_ms = 10;
    button_periodic_handler( HAL_GetTick() );
    return RET_OK;
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
    char errMsg[64];
    #ifdef DEBUG
        uint32_t timer_count, i = 0;
        LED_set(LED_0, 0);
        LED_set(LED_1, 1);
        sprintf(errMsg, "Err: %i", err_code);

        // Can't rely on HAL tick as we maybe in the ISR context...
        while (1) {
            log_logMessage(kLogLevelError, file, "?", line, errMsg);

            for (i = 0; i < 25; i++) {
                while (timer_count < 100000) {
                    timer_count++;
                }
                timer_count = 0;
                // If we're in debug mode, keep the watchdog kickin
                #if defined(DEBUG) && defined(WATCHDOG_ENABLE)
                    if (gPetWdg) {
                        wdg_pet();
                    }
                #endif
            }
            LED_toggle(LED_0);
            LED_toggle(LED_1);
        }
    #else
        // TODO: Reset everything
        log_logMessage(kLogLevelError, file, "?", line, errMsg);
        // for now, just let the watchdog happen
        while (1);
    #endif
}


// HAL uses this function. Call our error function.
void assert_failed(uint8_t* file, uint32_t line)
{
    fatal_error_handler((char *)file, line, -1);
}
