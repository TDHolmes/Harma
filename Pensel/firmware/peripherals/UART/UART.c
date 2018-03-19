/*!
 * @file    UART.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Wrapper functions around the stm32f3xx HAL UART functions.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "UART.h"

#include "modules/utilities/queue.h"
#include "modules/utilities/newqueue.h"

// STM driver includes
#include "peripherals/stm32f3-configuration/stm32f3xx.h"
#include "peripherals/stm32f3/stm32f3xx_hal_def.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"
#include "peripherals/stm32f3/stm32f3xx_hal_uart.h"

extern critical_errors_t gCriticalErrors;

//! We will have 3x UART TX buffers in a circular buffer
#define UART_NUM_TX_BUFFER (3)

// HAL UART handler declaration
UART_HandleTypeDef HAL_UART_handle;

//! UART structure to track RX / TX buffers
typedef struct {
    // define buffers to be used by the STM drivers
    uint8_t tx_buffer[UART_TX_BUFFER_SIZE];  //!< transmit buffer
    uint8_t rx_buffer[UART_RX_BUFFER_SIZE];  //!< receive buffer
    volatile queue_t rx_buffer_admin;   //!< queue_t admin to track RX circular buffer
    volatile newqueue_t tx_buffer_admin;   //!< queue_t admin to track RX circular buffer

} UART_admin_t;

//! UART admin
UART_admin_t UART_admin;


/*! UART initialization function
 *
 * @param  baudrate (uint32_t): Speed of transactions
 * @retval Return code indicating success / failure of init
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
    queue_init(&UART_admin.rx_buffer_admin);
    newqueue_init(&UART_admin.tx_buffer_admin, UART_TX_BUFFER_SIZE * UART_NUM_TX_BUFFER, 1);
    HAL_UART_Receive_IT(&HAL_UART_handle, UART_admin.rx_buffer, 1);

    // yay we're done!
    return RET_OK;
}

/*! Checks if the UART TX line is busy sending things
 *
 * @retval Boolean indicating if UART TX is ready to send
 */
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


/*! Function that waits on the UART driver to be ready with a `UART_TIMEOUT_MS` ms
 * timeout.
 *
 * @retval Return code indicating success / failure of the start of the transmit
 */
ret_t UART_waitForReady_withTimeout(void)
{
    uint32_t timeout = HAL_GetTick() + UART_TIMEOUT_MS;
    while( UART_TXisReady() == false ) {
        if ( HAL_GetTick() > timeout ) {
            return RET_BUSY_ERR;
        }
    }
    return RET_OK;
}


/*! Sends a buffer of data over UART
 *
 * @param data_ptr (uint8_t *): pointer to the data you want to transmit
 * @param num_bytes (uint8_t): amount of bytes you want to send
 * @retval Return code indicating success / failure of the start of the transmit
 *
 * @Note Can only send a maximum of `UART_TX_BUFFER_SIZE`
 */
ret_t UART_sendData(uint8_t * data_ptr, uint8_t num_bytes)
{
    // next, transmit the data if we aren't already busy
    if ( UART_TXisReady() ) {

        // first, make sure we have enough room!
        if (num_bytes > UART_TX_BUFFER_SIZE) {
            // Transmit the bit we can, queue up the rest.
            memcpy(UART_admin.tx_buffer, data_ptr, UART_TX_BUFFER_SIZE);
            if (HAL_UART_Transmit_IT(&HAL_UART_handle, UART_admin.tx_buffer, UART_TX_BUFFER_SIZE) != HAL_OK) {
                return RET_COM_ERR;
            }

            // queue the rest
            gCriticalErrors.UART_queuedBytes += num_bytes - UART_TX_BUFFER_SIZE;
            return newqueue_push(
                &UART_admin.tx_buffer_admin,
                data_ptr + UART_TX_BUFFER_SIZE,
                num_bytes - UART_TX_BUFFER_SIZE);
        } else {
            // We can transmit all of it in one go
            memcpy(UART_admin.tx_buffer, data_ptr, num_bytes);
            if (HAL_UART_Transmit_IT(&HAL_UART_handle, UART_admin.tx_buffer, num_bytes) != HAL_OK) {
                return RET_COM_ERR;
            }
        }
    } else {

        // queue it up for later transmission
        if (UART_admin.tx_buffer_admin.num_items - UART_admin.tx_buffer_admin.unread_items >= num_bytes) {
            // We have enough space to queue this data up.
            gCriticalErrors.UART_queuedBytes += num_bytes;
            return newqueue_push(&UART_admin.tx_buffer_admin, data_ptr, num_bytes);
        } else {
            // No room. Try again later.
            gCriticalErrors.UART_droppedBytes += num_bytes;
            return RET_BUSY_ERR;

        }
    }
    return RET_OK;
}

/*! Sends a single byte of data over UART (Blocking)
 *
 * @param data (uint8_t): Byte to send
 * @retval Return code indicating success / failure of the start of the transmit
 *
 * @note this function will timeout after `UART_TIMEOUT_MS` miliseconds of the
 *      HAL UART driver being busy.
 */
ret_t UART_sendChar(uint8_t data)
{
    ret_t ready_result = UART_waitForReady_withTimeout();
    if (ready_result != RET_OK) {
        return ready_result;
    }
    return UART_sendData(&data, 1);
}

/*! Send a string via UART with a maximum length possible of 255
 *
 * @param string_ptr (char[]): Pointer to the string to be sent
 * @retval Return code indicating success / failure of the start of the transmit
 *
 * @Note This function expects a null terminated string.
 */
ret_t UART_sendString(char string_ptr[])
{
    ret_t retval;
    uint8_t max_len = 255;
    while (*string_ptr != 0) {
        // send the character
        retval = UART_sendData((uint8_t *)string_ptr, 1);
        if (retval != RET_OK) { return retval; }
        string_ptr += 1;
        max_len -= 1;
        if (max_len == 0) {
            return RET_MAX_LEN_ERR;
        }
    }
    return RET_OK;
}

/*! Sends an integer via UART encoded in ascii hexadecimal
 *
 * @param data (int64_t): Data to be sent. Must be cast to `int64_t`
 * @retval Return code indicating success / failure of the start of the transmit
 */
ret_t UART_sendint(int64_t data)
{
    ret_t retval;
    int8_t shift_val = 48;
    uint8_t val = 0;
    uint8_t chr = 0;
    if (data < 0) {
        UART_sendChar(0x2d);  // "-"
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

            chr = val & 0xF;
            if (chr >= 0xA) {
                chr += 0x37;  // into A-F
            } else {
                chr += 0x30;  // into 0-9
            }
            retval = UART_sendChar(chr);
            if (retval != RET_OK) { return retval; }
        }
        shift_val -= 8;
    }
    return RET_OK;
}

/*! Sends an float value via UART encoded in ascii using `sprintf`
 *
 * @param data (float): The value to be printed
 * @param percision (uint8_t): number of fractional places to print
 * @retval Return code indicating success / failure of the start of the transmit
 *
 * @Note This function does not use the %f specifier, but rather does "%ld.%ld"
 *      and calculates the whole and fractional part
 */
ret_t UART_sendfloat(float data, uint8_t percision)
{
    // TODO: Is 40 bytes too much / little?
    char char_buff[40];
    int32_t significand;
    int32_t fractional;

    significand = (int32_t)data;
    fractional = (int32_t)((data - (int32_t)data) * (10.0f * (float)percision));
    sprintf(char_buff, "%d.%d", significand, fractional);

    UART_sendString(char_buff);
    return RET_OK;
}

/*! Gets a byte from the recieved data buffer.
 *
 * @param data_ptr (uint8_t *): Place to store the retreived byte
 * @retval Return code indicating success / failure of getting the byte of data
 */
ret_t UART_getChar(uint8_t * data_ptr)
{
    // check if we have any data to give
    if (UART_dataAvailable()) {
        // copy the data into the given pointer!
        *data_ptr = UART_admin.rx_buffer[UART_admin.rx_buffer_admin.tail_ind];
        queue_increment_tail(&UART_admin.rx_buffer_admin, UART_RX_BUFFER_SIZE);
        return RET_OK;
    } else {
        return RET_NODATA_ERR;
    }
}

/*! Gets a byte from the recieved data buffer, but doesn't take it off the buffer.
 *
 * @param data_ptr (uint8_t *): Place to store the retreived byte
 * @retval Return code indicating success / failure of getting the byte of data
 */
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

/*! Checks if there is data in the RX buffer.
 *
 * @retval Boolean as to whether or not there is unread data in the buffer.
 */
bool UART_dataAvailable(void)
{
    if (UART_admin.rx_buffer_admin.unread_items != 0) {
        return true;
    } else {
        return false;
    }
}

/*! Returns how many bytes have been overwritten in the RX buffer.
 *
 * @uint8_t Number of dropped/overwritten bytes
 */
uint8_t UART_droppedPackets(void)
{
    return UART_admin.rx_buffer_admin.overwrite_count;
}


/*! Rx Transfer completed callback. Push onto the rx buffer!
 *
 * @param  HAL_UART_handle: UART handle
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *HAL_UART_handle)
{

    // New data was put onto the buffer by the ISR!
    // move the head of the buffer forward
    queue_increment_head(&UART_admin.rx_buffer_admin, UART_RX_BUFFER_SIZE);

    // start recieving data again @ the head index
    HAL_UART_Receive_IT(HAL_UART_handle, &UART_admin.rx_buffer[UART_admin.rx_buffer_admin.head_ind], 1);
}


/*! Tx Transfer completed callback. Pop from the tx buffer if available!
 *
 * @param  HAL_UART_handle: UART handle
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *HAL_UART_handle)
{
    if (UART_admin.tx_buffer_admin.unread_items != 0) {
        uint32_t num_bytes;
        HAL_StatusTypeDef hal_retval;

        if (UART_admin.tx_buffer_admin.unread_items <= UART_TX_BUFFER_SIZE) {
            num_bytes = UART_admin.tx_buffer_admin.unread_items;
        } else {
            num_bytes = UART_TX_BUFFER_SIZE;
        }

        gCriticalErrors.UART_dequeuedBytes += num_bytes;
        newqueue_pop(&UART_admin.tx_buffer_admin, UART_admin.tx_buffer, num_bytes, eNoPeak);

        hal_retval = HAL_UART_Transmit_IT(HAL_UART_handle, UART_admin.tx_buffer, num_bytes);
        if (hal_retval != HAL_OK) {
            fatal_error_handler(__FILE__, __LINE__, hal_retval);
        }
    }
}

/*! UART error callback. Call `fatal_error_handler` and go into infinite loop
 *
 * @param  HAL_UART_handle: UART handle
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *HAL_UART_handle)
{
    fatal_error_handler(__FILE__, __LINE__, HAL_UART_handle->ErrorCode);
}
