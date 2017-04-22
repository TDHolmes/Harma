/*
 *
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "UART.h"

// STM driver includes
#include "stm32f3xx.h"
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"
#include "Drivers/stm32f3xx_hal_uart.h"


#define RX_BUFFER_SIZE (10)
#define TX_BUFFER_SIZE (10)

#define RX_QUEUE_SIZE (3)


// HAL UART handler declaration
UART_HandleTypeDef UartHandle;

typedef struct {
    // define buffers to be used by the STM drivers
    uint8_t rx_buffer[RX_BUFFER_SIZE];
    uint8_t tx_buffer[TX_BUFFER_SIZE];
    uint8_t rx_packet_queue[RX_BUFFER_SIZE * RX_QUEUE_SIZE];
    uint8_t rx_packet_count;
    uint8_t rx_droppedpackets_count;

    uint8_t sending_data;
} UART_admin_t;

UART_admin_t UART_admin;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
ret_t UART_init(uint32_t baudrate)
{

    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
    UartHandle.Instance        = USART1;

    UartHandle.Init.BaudRate   = baudrate;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if(HAL_UART_Init(&UartHandle) != HAL_OK) {
        return RET_COM_ERR;
    }

    // Start receiving data!
    HAL_UART_Receive_IT(&UartHandle, UART_admin.rx_buffer, RX_BUFFER_SIZE);

    // yay we're done!
    return RET_OK;
}


bool UART_isReady(void)
{
    if (UART_admin.sending_data == 0) {
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
    if (UART_admin.sending_data == 0) {

        memcpy(UART_admin.tx_buffer, data_ptr, num_bytes);
        if (HAL_UART_Transmit_IT(&UartHandle, UART_admin.tx_buffer, num_bytes) != HAL_OK) {
            return RET_COM_ERR;
        }
        UART_admin.sending_data = 1;
    } else {
        return RET_BUSY_ERR;
    }
    return RET_OK;
}


ret_t UART_getData(uint8_t * pkt_buffer)
{
    uint8_t * src_ptr;

    // check if we have any data to give
    if (UART_dataAvailable()) {
        // copy the data into the given pointer!
        src_ptr = UART_admin.rx_packet_queue + (UART_admin.rx_packet_count - 1) * RX_BUFFER_SIZE;
        memcpy(pkt_buffer, src_ptr, RX_BUFFER_SIZE);
        UART_admin.rx_packet_count -= 1;
        return RET_OK;
    } else {
        return RET_NODATA_ERR;
    }
}


bool UART_dataAvailable(void)
{
    if (UART_admin.rx_packet_count != 0) {
        return true;
    } else {
        return false;
    }
}


uint8_t UART_droppedPackets(void)
{
    return UART_admin.rx_droppedpackets_count;
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    // indicate we're done transmitting data!
    UART_admin.sending_data = 0;
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    uint8_t * dest_ptr;

    // copy the received packet into the packet queue!
    if (UART_admin.rx_packet_count < RX_QUEUE_SIZE) {
        dest_ptr = UART_admin.rx_packet_queue + (UART_admin.rx_packet_count * RX_BUFFER_SIZE);
        memcpy(dest_ptr, UART_admin.rx_buffer, RX_BUFFER_SIZE);
        UART_admin.rx_packet_count += 1;
    } else {
        // Oh no! we don't have any space. packet gets dropped :'(
        UART_admin.rx_droppedpackets_count += 1;
    }
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
    fatal_error_handler();
}
