/*
 *
 *
 */

#include <stdint.h>
#include <string.h>
#include "common.h"
#include "hardware.h"

// Sensors
#include "LSM303DLHC.h"
// TODO: add button and thumbwheel support
#include "ADC.h"  // for thumbwheel

// Communications and other
#include "I2C.h"
#include "UART.h"
#include "cli.h"

// STM Drivers
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"
#include "stm32f3xx_hal_conf.h"


const uint8_t hello_world_str[] = {"Hello!!!\n"};
uint8_t data_to_send[10];


extern __IO uint32_t uwTick;


int main(void)
{
    ret_t retval;
    uint32_t prev_tick = 0;

    // system configuration...
    HAL_Init();
    SystemClock_Config();
    configure_pins();

    // peripheral configuration
    retval = I2C_init();
    if (retval != RET_OK) {
        fatal_error_handler();
    }
    retval = UART_init(115200);
    if (retval != RET_OK) {
        fatal_error_handler();
    }

    retval = ADC_init();
    if (retval != RET_OK) {
        fatal_error_handler();
    }

    retval = cli_init(UART_sendChar, UART_getChar);
    if (retval != RET_OK) {
        fatal_error_handler();
    }

    while (true) {
        // toggle LED_0 every second
        if (HAL_GetTick() - prev_tick > 1000) {
            LED_toggle(LED_0);
            prev_tick = HAL_GetTick();
        }

        // toggle LED_1 on UART TX
        if (UART_isReady()) {

            // check if we should reflect what we've recieved
            if ( UART_dataAvailable() ) {
                UART_peakChar(data_to_send);
                cli_run();
                UART_sendChar(*data_to_send);
            } else {
                memcpy(data_to_send, hello_world_str, sizeof(hello_world_str));
                UART_sendData(data_to_send, 10);
            }
            LED_toggle(LED_1);
        }
    }
}

/*! millisecond ISR that increments the global time as well as calls some functions
 *  that need to be periodically serviced.
 */
void HAL_IncTick(void)
{
    // sub counter to take care of some tasks every N ms
    static uint8_t sub_count = 0;

    // increment the ms timer
    uwTick++;

    // take care of some things that need to be called periodically
    if (sub_count == 9) {
        sub_count = 0;
        button_periodic_handler(uwTick);
        switch_periodic_handler(uwTick);
    } else {
        sub_count++;
    }
}


// all components call this function if there is a fatal error
void fatal_error_handler(void)
{
    // do something
    while (1);
}

// TODO: serial print file and line of assert failure
void assert_failed(uint8_t* file, uint32_t line)
{
    fatal_error_handler();
}
