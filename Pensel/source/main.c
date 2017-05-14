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


const uint8_t hello_world_str[] = {"Hello!!!\r\n"};
uint8_t data_to_send[10];


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

    // peripheral configuration
    retval = I2C_init();
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    retval = ADC_init();
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    retval = LSM303DLHC_init(kAccelODR_100_Hz, kOne_mg_per_LSB, kMagODR_75_Hz, kXY_230_Z_205_LSB_per_g);
    if (retval != RET_OK) {
        fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    }

    // retval = cli_init(UART_sendChar, UART_getChar);
    // if (retval != RET_OK) {
    //     fatal_error_handler(__FILE__, __LINE__, (int8_t)retval);
    // }

    while (true) {
        // if (HAL_GetTick() - prev_tick > 5000) {
        //     prev_tick = HAL_GetTick();
        //     UART_sendString("Accel Overwrite Count: ");
        //     UART_sendint( LSM303DLHC_accel_packetOverwriteCount() );
        //     UART_sendString("\r\n");
        //     UART_sendString("Mag Overwrite Count: ");
        //     UART_sendint( LSM303DLHC_mag_packetOverwriteCount() );
        //     UART_sendString("\r\n");
        //     UART_sendString("Accel HW Overwrite Count: ");
        //     UART_sendint( LSM303DLHC_accel_packetOverwriteCount() );
        //     UART_sendString("\r\n");
        //     UART_sendString("Mag HW Overwrite Count: ");
        //     UART_sendint( LSM303DLHC_mag_packetOverwriteCount() );
        //     UART_sendString("\r\n");
        // }
        if( LSM303DLHC_accel_dataAvailable() ) {
            if ( LSM303DLHC_accel_getPacket(&accel_pkt, false) == RET_OK ) {
                UART_sendString("Acc ");
                UART_sendint((int64_t)accel_pkt.x);
                UART_sendString(" ");
                UART_sendint((int64_t)accel_pkt.y);
                UART_sendString(" ");
                UART_sendint((int64_t)accel_pkt.z);
                UART_sendString("\r\n");
            } else {
                UART_sendString("Failed to get accel packet...\r\n");
            }
        }
        if( LSM303DLHC_mag_dataAvailable() ) {
            if ( LSM303DLHC_mag_getPacket(&mag_pkt, false) == RET_OK ) {
                UART_sendString("Mag ");
                UART_sendint((int64_t)mag_pkt.x);
                UART_sendString(" ");
                UART_sendint((int64_t)mag_pkt.y);
                UART_sendString(" ");
                UART_sendint((int64_t)mag_pkt.z);
                UART_sendString("\r\n");
            } else {
                UART_sendString("Failed to get mag packet...\r\n");
            }
        }

        // toggle LED_1 on UART TX
        // if (UART_isReady()) {
        //     // check if we should reflect what we've recieved
        //     if ( UART_dataAvailable() ) {
        //         UART_getChar(data_to_send);
        //         cli_run();
        //         UART_sendChar(*data_to_send);
        //     }
        // }
    }
}

/*! millisecond ISR that increments the global time as well as calls some functions
 *  that need to be periodically serviced.
 */
void HAL_IncTick(void)
{
    // sub counter to take care of some tasks every N ms
    static uint8_t sub_count = 0;
    static uint16_t second_count = 0;

    // increment the ms timer
    uwTick++;

    // take care of some things that need to be called periodically
    if (sub_count >= 9) {
        sub_count = 0;
        button_periodic_handler(uwTick);
        switch_periodic_handler(uwTick);
    } else {
        sub_count++;
    }

    if (second_count >= 1000) {
        second_count = 0;
        LED_toggle(LED_0);
    } else {
        second_count++;
    }
}


// all components call this function if there is a fatal error
void fatal_error_handler(char file[], uint32_t line, int8_t err_code)
{
    // FREAK OUT
    LED_set(LED_0, 0);
    LED_set(LED_1, 1);
    while (1) {
        UART_sendString(file);
        UART_sendString(":");
        UART_sendint((int64_t)line);
        UART_sendString(" threw ");
        UART_sendint((int64_t)err_code);
        UART_sendString("\r\n");
        HAL_Delay(100U);
        LED_toggle(LED_0);
        LED_toggle(LED_1);
    }
}

// HAL uses this function. Call our error function
void assert_failed(uint8_t* file, uint32_t line)
{
    fatal_error_handler((char *)file, line, 0);
}
