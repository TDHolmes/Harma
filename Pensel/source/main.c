/*
 *
 *
 */

#include <stdint.h>
#include "common.h"
#include "hardware.h"

// Sensors
#include "LSM303DLHC.h"
// TODO: add button and thumbwheel support
#include "ADC.h"  // for thumbwheel

// Communications and other
#include "I2C.h"
#include "UART.h"

// STM Drivers
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"
#include "stm32f3xx_hal_conf.h"


uint8_t data_buff[] = {"Hello, World!\n"};


int main(void)
{
    ret_t retval;
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


    while (true) {
        LED_toggle(LED_0);
        if (UART_isReady()) {
            LED_toggle(LED_1);
            UART_sendData(data_buff, 14);
        }
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
