/*!
 * @file    main.c
 * @author  Tyler Holmes
 * @version 0.1.0
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
#include "modules/orientation/orientation.h"
#include "modules/calibration/cal.h"
#include "modules/utilities/scheduler.h"

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

PCD_HandleTypeDef hpcd_USB_FS;

//! HAL millisecond tick
extern __IO uint32_t uwTick;
// Global variables to influence state

critical_errors_t gCriticalErrors;
schedule_t main_schedule;

// Local functions
ret_t heartbeat(int32_t * new_callback_time_ms);
ret_t workloop_flash(int32_t * new_callback_time_ms);
ret_t button_handler(int32_t * new_callback_time_ms);
void USB_pullup_set(uint8_t value);


#ifdef WATCHDOG_ENABLE
    //! Global indicating whether or not to pet the watchdog
    static bool gPetWdg = false;

    void wdg_captureAlert(void) {
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

    ret_t watchdog_pet(int32_t * new_callback_time_ms) {
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

extern __IO uint8_t receive_buffer[64];
extern __IO uint32_t receive_length;
extern __IO uint32_t length;
uint8_t send_buffer[64];
uint32_t packet_sent = 1;
uint32_t packet_receive = 1;


/* USB init function */
void MX_USB_PCD_Init(void)
{
    hpcd_USB_FS.Instance = USB;
    hpcd_USB_FS.Init.dev_endpoints = 8;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.ep0_mps = DEP0CTL_MPS_64;
    hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;

    HAL_StatusTypeDef ret;
    ret = HAL_PCD_Init(&hpcd_USB_FS);
    if (ret != HAL_OK) {
        check_retval_fatal(__FILE__, __LINE__, (ret_t)ret);
    }
}


void USB_pullup_set(uint8_t value)
{
    if (value) {
        HAL_GPIO_WritePin(USB_PORT, USB_DP_PULLUP, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(USB_PORT, USB_DP_PULLUP, GPIO_PIN_RESET);
    }
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

    // system configuration...
    HAL_Init();
    SystemClock_Config();
    configure_pins();
    clear_critical_errors();

    // Initialize UART
    check_retval_fatal(__FILE__, __LINE__, UART_init(115200) );

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
    // retval = I2C_init();
    // check_retval_fatal(__FILE__, __LINE__, retval);

    UART_sendString("\r\n");
    LED_set(LED_0, 0);
    LED_set(LED_1, 0);

    // disable WWDG
    WRITE_REG(WWDG->CR, 0);

    UART_sendString("before USB\r\n");
    while( !UART_TXisReady() );
    MX_USB_PCD_Init();
    USB_Init();
    UART_sendString("asserting USB pullup...\r\n");
    while( !UART_TXisReady() );
    USB_pullup_set(1);
    UART_sendString("USB pulled up.\r\n");
    while( !UART_TXisReady() );

    uint32_t ledi = 0;
    while (1)
    {

        // toggle LED every 10,000 itterations over the workloop
        if (ledi++ >= 10000) {
            UART_sendString("toggle\r\n");
            LED_toggle(LED_1);
            ledi = 0;
        }
        // UART_sendint(bDeviceState);
        if (bDeviceState == CONFIGURED) {
            UART_sendString("configured\r\n");
            CDC_Receive_DATA();
            /*Check to see if we have data yet */
            if (receive_length  != 0) {
                if (packet_sent == 1)
                    CDC_Send_DATA ((unsigned char*)receive_buffer,receive_length);
                receive_length = 0;
            }
        }
    }

    // initalize the scheduler and add some periodic tasks
    scheduler_init(&main_schedule);
    scheduler_add(&main_schedule, 0, heartbeat, &i);
    scheduler_add(&main_schedule, 0, workloop_flash, &i);
    scheduler_add(&main_schedule, 0, button_handler, &i);
    #ifdef WATCHDOG_ENABLE
        scheduler_add(&main_schedule, 0, watchdog_pet, &i);
    #endif

    UART_sendString("Initialized\r\n");

    uint32_t sleep_until = 0;
    while (true) {
        sleep_until = HAL_GetTick();  // grab start of loop time
        time_until_next_cb_ms = scheduler_run(&main_schedule, sleep_until);
        sleep_until += time_until_next_cb_ms;

        if (time_until_next_cb_ms > 0) {
            UART_sendString("Sleeping for "); UART_sendint(time_until_next_cb_ms); UART_sendString(" ms\r\n");
            // TODO: sleep instead of dumb delay
            if (HAL_GetTick() < sleep_until ) {
                HAL_Delay( sleep_until - HAL_GetTick() );
            }
        }
    } /* while (true) */
} /* main() */


/* --- Some common callbacks to be run --- */

ret_t heartbeat(int32_t * new_callback_time_ms) {
    static uint8_t tick = 0;
    *new_callback_time_ms = 1000;
    LED_toggle(LED_0);
    TimingPin_toggle();

    if (tick % 2 == 0) {
        UART_sendString("\tTick\r\n");
    } else if (tick % 2 == 1) {
        UART_sendString("\tTock\r\n");
    }
    tick += 1;
    return RET_OK;
}

ret_t workloop_flash(int32_t * new_callback_time_ms) {
    static uint32_t i = 0;
    *new_callback_time_ms = 0;

    // toggle LED every 10,000 itterations over the workloop
    if (i++ >= 10000) {
        LED_toggle(LED_1);
        i = 0;
    }
    return RET_OK;
}


ret_t button_handler(int32_t * new_callback_time_ms) {
    UART_sendString("b");
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
