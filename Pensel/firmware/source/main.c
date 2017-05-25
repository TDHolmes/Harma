/*
 *
 *
 */

#include <stdint.h>
#include <string.h>
#include "common.h"

// Sensors
#include "LSM303DLHC.h"
#include "ADC.h"  // for thumbwheel
#include "hardware.h" // HW support and button / switch functions

// Communications and other
#include "I2C.h"
#include "UART.h"
#include "reports.h"

// STM Drivers
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"
#include "stm32f3xx_hal_conf.h"


const uint8_t hello_world_str[] = {"Hello!!!\r\n"};
uint8_t data_to_send[10];

//! HAL millisecond tick
extern __IO uint32_t uwTick;


int main(void)
{
    ret_t retval;
    // uint32_t prev_tick = 0;
    // switch_state_t switch_state = kSwitch_0;
    // uint8_t main_btn_state = 0;
    // uint8_t aux_btn_state = 0;
    mag_packet_t mag_pkt;
    accel_packet_t accel_pkt;

    // system configuration...
    HAL_Init();
    SystemClock_Config();
    configure_pins();

    retval = UART_init(115200);
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    retval = wdg_init();
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    // peripheral configuration
    retval = I2C_init();
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    retval = ADC_init();
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    retval = LSM303DLHC_init(kAccelODR_100_Hz, kOne_mg_per_LSB, kMagODR_75_Hz, kXY_1100_Z_980_LSB_per_g);
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    // retval = rpt_init(UART_sendChar, UART_getChar);
    // if (retval != RET_OK) {
    //     fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    // }

    while (true) {
        if( LSM303DLHC_accel_dataAvailable() ) {
            if ( LSM303DLHC_accel_getPacket(&accel_pkt, false) == RET_OK ) {
                UART_sendString("Acc ");
                UART_sendfloat(accel_pkt.x, 3);
                UART_sendString(", ");
                UART_sendfloat(accel_pkt.y, 3);
                UART_sendString(", ");
                UART_sendfloat(accel_pkt.z, 3);
                UART_sendString("\r\n");
            } else {
                UART_sendString("Failed to get accel packet...\r\n");
            }
        }
        if( LSM303DLHC_mag_dataAvailable() ) {
            if ( LSM303DLHC_mag_getPacket(&mag_pkt, false) == RET_OK ) {
                UART_sendString("Mag ");
                UART_sendfloat(mag_pkt.x, 3);
                UART_sendString(" ");
                UART_sendfloat(mag_pkt.y, 3);
                UART_sendString(" ");
                UART_sendfloat(mag_pkt.z, 3);
                UART_sendString("\r\n");
            } else {
                UART_sendString("Failed to get mag packet...\r\n");
            }
        }
    }
}

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
    // sub counter to take care of some tasks every N ms
    static uint8_t sub_count = 0;
    static uint16_t second_count = 0;

    // increment the ms timer
    uwTick++;

    // take care of some things that need to be called periodically every ~10 ms
    if (sub_count >= 9) {
        sub_count = 0;
        button_periodic_handler(uwTick);
        switch_periodic_handler(uwTick);
    } else {
        sub_count++;
    }

    // every 5 ms, pet the watchdog. Need to pet the watchdog every 10 ms!
    if (sub_count == 5) {
        wdg_pet();
    }

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
 * @param err_code (int8_t): Error code that was thrown. Usually the ret_t value that caused the fail.
 */
void fatal_error_handler(char file[], uint32_t line, int8_t err_code)
{
    // FREAK OUT
    uint8_t i;
    LED_set(LED_0, 0);
    LED_set(LED_1, 1);
    while (1) {
        /* TODO: Switch to logging the error to a logging system and allow
         *   watchdog to reset us when in a release compile */
        UART_sendString(file);
        UART_sendString(":");
        UART_sendint((int64_t)line);
        UART_sendString(" threw ");
        UART_sendint((int64_t)err_code);
        UART_sendString("\r\n");
        for (i = 0; i < 25; i++) {
            HAL_Delay(5U);
            // If we're in debug mode, keep the watchdog kickin
            #ifdef DEBUG
                wdg_pet();
            #endif
        }
        LED_toggle(LED_0);
        LED_toggle(LED_1);
    }
}

// HAL uses this function. Call our error function.
void assert_failed(uint8_t* file, uint32_t line)
{
    fatal_error_handler((char *)file, line, -1);
}
