/*
 *
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "UART.h"
#include "queue.h"

// STM driver includes
#include "stm32f3xx.h"
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"
#include "Drivers/stm32f3xx_hal_uart.h"


#define RX_BUFFER_SIZE (40)
#define TX_BUFFER_SIZE (10)


// HAL UART handler declaration
UART_HandleTypeDef HAL_UART_handle;

typedef struct {
    // define buffers to be used by the STM drivers
    uint8_t tx_buffer[TX_BUFFER_SIZE];
    uint8_t rx_buffer[RX_BUFFER_SIZE];
    volatile queue_t rx_buffer_admin;

    // volatile uint8_t sending_data;
} UART_admin_t;

UART_admin_t UART_admin;

/*-------------------------- Private functions -------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
ret_t UART_init(uint32_t baudrate)
{

    // configure the UART module we are using.
    HAL_UART_handle.Instance        = USART1;
    HAL_UART_handle.Init.BaudRate   = baudrate;
    HAL_UART_handle.Init.WordLength = UART_WORDLENGTH_8B;
    HAL_UART_handle.Init.StopBits   = UART_STOPBITS_1;
    HAL_UART_handle.Init.Parity     = UART_PARITY_NONE;
    HAL_UART_handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    HAL_UART_handle.Init.Mode       = UART_MODE_TX_RX;
    HAL_UART_handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    // configure our own admin
    // UART_admin.sending_data = 0;

    if(HAL_UART_Init(&HAL_UART_handle) != HAL_OK) {
        return RET_COM_ERR;
    }

    // Start receiving data!
    HAL_UART_Receive_IT(&HAL_UART_handle, UART_admin.rx_buffer, 1);

    // yay we're done!
    return RET_OK;
}


bool UART_TXisReady(void)
{
    HAL_UART_StateTypeDef state;
    state = HAL_UART_GetState(&HAL_UART_handle);

    if (state != HAL_UART_STATE_BUSY_TX_RX && state != HAL_UART_STATE_BUSY_TX) {
        return true;
    } else {
        return false;
    }
}


ret_t UART_sendData(uint8_t * data_ptr, uint8_t num_bytes)
{
    // first, make sure we have enough room!
    if (num_bytes > TX_BUFFER_SIZE) {
        return RET_LEN_ERR;
    }

    // next, transmit the data if we aren't already busy
    if ( UART_TXisReady() ) {

        memcpy(UART_admin.tx_buffer, data_ptr, num_bytes);
        if (HAL_UART_Transmit_IT(&HAL_UART_handle, UART_admin.tx_buffer, num_bytes) != HAL_OK) {
            return RET_COM_ERR;
        }
    } else {
        return RET_BUSY_ERR;
    }
    return RET_OK;
}


ret_t UART_sendChar(uint8_t data)
{
    while( UART_TXisReady() == false );
    return UART_sendData(&data, 1);
}

/* Send a string via UART with a maximum length possible of 255
 *
 */
ret_t UART_sendString(char string_ptr[])
{
    ret_t retval;
    uint8_t max_len = 255;
    while (*string_ptr != 0) {
        // send the character
        retval = UART_sendChar(*string_ptr);
        if (retval != RET_OK) { return retval; }
        string_ptr += 1;
        max_len -= 1;
        if (max_len == 0) {
            return RET_MAX_LEN_ERR;
        }
    }
    return RET_OK;
}


ret_t UART_sendint(int64_t data)
{
    ret_t retval;
    int8_t shift_val = 48;
    uint8_t val = 0;
    uint8_t chr = 0;
    if (data < 0) {
        UART_sendChar(0x2d);  // "-"
        while( UART_TXisReady() == false );
        data = -data;
    }
    UART_sendString("0x");

    while (shift_val >= 0) {
        // send the character
        val = (data >> shift_val) & 0xff;
        if (val != 0 || shift_val == 0) {
            chr = (val & 0xF0) >> 4;
            if (chr >= 0xA) {
                chr += 0x37;  // into A-F
            } else {
                chr += 0x30;  // into 0-9
            }
            retval = UART_sendChar(chr);
            if (retval != RET_OK) { return retval; }
            // wait for UART to be ready
            while( UART_TXisReady() == false );
            chr = val & 0xF;
            if (chr >= 0xA) {
                chr += 0x37;  // into A-F
            } else {
                chr += 0x30;  // into 0-9
            }
            retval = UART_sendChar(chr);
            if (retval != RET_OK) { return retval; }
            // wait for UART to be ready
            while( UART_TXisReady() == false );
        }
        shift_val -= 8;
    }
    return RET_OK;
}


ret_t UART_getChar(uint8_t * data_ptr)
{
    // check if we have any data to give
    if (UART_dataAvailable()) {
        // copy the data into the given pointer!
        *data_ptr = UART_admin.rx_buffer[UART_admin.rx_buffer_admin.tail_ind];
        queue_increment_tail(&UART_admin.rx_buffer_admin, RX_BUFFER_SIZE);
        return RET_OK;
    } else {
        return RET_NODATA_ERR;
    }
}


ret_t UART_peakChar(uint8_t * data_ptr)
{
    // check if we have any data to give
    if (UART_dataAvailable()) {
        // copy the data into the given pointer!
        *data_ptr = UART_admin.rx_buffer[UART_admin.rx_buffer_admin.tail_ind];
        // don't mess with the tail index
        return RET_OK;
    } else {
        return RET_NODATA_ERR;
    }
}


bool UART_dataAvailable(void)
{
    if (UART_admin.rx_buffer_admin.unread_items != 0) {
        return true;
    } else {
        return false;
    }
}


uint8_t UART_droppedPackets(void)
{
    return UART_admin.rx_buffer_admin.overwrite_count;
}

/**
  * @brief  Tx Transfer completed callback
  * @param  HAL_UART_handle: UART handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *HAL_UART_handle)
{
    // indicate we're done transmitting data!
    // UART_admin.sending_data = 0;
}

/**
  * @brief  Rx Transfer completed callback
  * @param  HAL_UART_handle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *HAL_UART_handle)
{

    // New data is on the buffer!
    // move the head of the buffer forward
    queue_increment_head(&UART_admin.rx_buffer_admin, RX_BUFFER_SIZE);

    // start recieving data again
    HAL_UART_Receive_IT(HAL_UART_handle, &UART_admin.rx_buffer[UART_admin.rx_buffer_admin.head_ind], 1);
}

/**
  * @brief  UART error callbacks
  * @param  HAL_UART_handle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *HAL_UART_handle)
{
    fatal_error_handler(__FILE__, __LINE__, 0);
}
