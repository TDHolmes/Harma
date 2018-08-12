/*!
 * @file    I2C.c
 * @author  Tyler Holmes
 *
 * @date    20-May-2017
 * @brief   Wrapper functions around the stm32f3xx HAL I2C functions.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "I2C.h"

// STM HAL libraries
#include "stm32f3xx.h"
#include "peripherals/stm32f3/stm32f3xx_hal_def.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"
#include "peripherals/stm32f3/stm32f3xx_hal_i2c.h"



/* Size of Transmission buffers */
#define TX_BUFFER_SIZE                      (10)
#define RX_BUFFER_SIZE                      TX_BUFFER_SIZE

// Our I2C address
#define I2C_ADDRESS        (0x3F)

/* I2C TIMING Register define when I2C clock source is SYSCLK
   I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 64 MHz
   This example use TIMING to 0x00400B27 to reach 1 MHz speed (Rise time = 26ns, Fall time = 2ns) */
/*
PRESC     0xB
SCLL      0x13
tSCLL     20 x 250 ns = 5.0 μs
SCLH      0xF
tSCLH     16 x 250 ns = 4.0 μs
tSCL(1)   ~10 us
SDADEL    0x2
tSDADEL   2 x 250 ns = 500 ns
SCLDEL    0x4
tSCLDEL   5 x 250 ns = 1250 ns
*/
// PRESC[3:0] Res[3:0] SCLDEL[3:0] SDADEL[3:0] SCLH[7:0] SCLL[7:0]
// NOTE: I2C speed is 400 kHz with this setting. It was found by trial/error. double check?
#define I2C_TIMING      0x30420f13

/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;


// Buffers used for transmission
uint8_t TX_buffer[TX_BUFFER_SIZE];
uint8_t RX_buffer[RX_BUFFER_SIZE];


ret_t I2C_init(void)
{
    HAL_StatusTypeDef retval;

    // Configure the I2C HAL
    I2cHandle.Instance             = I2C1;
    I2cHandle.Init.Timing          = I2C_TIMING;
    I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress2     = 0xFF;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    retval = HAL_I2C_Init(&I2cHandle);
    if(retval != HAL_OK)
    {
        // Initialization error!
        fatal_error_handler(__FILE__, __LINE__, retval);
    }

    retval = HAL_I2CEx_ConfigAnalogFilter(&I2cHandle, I2C_ANALOGFILTER_ENABLE);
    if (retval != HAL_OK) {
        fatal_error_handler(__FILE__, __LINE__, retval);
    }

    return RET_OK;
}


ret_t I2C_writeData(uint8_t dev_address, uint8_t mem_address,
                    uint8_t * data_ptr, uint8_t data_len, bool blocking)
{
    // check if we're available to send data
    if (I2C_isBusy()) {
        return RET_BUSY_ERR;
    }

    // make sure the length is possible
    if (data_len > TX_BUFFER_SIZE) {
        return RET_LEN_ERR;
    }
    // copy the data into the TX buffer and send it!
    memcpy(TX_buffer, data_ptr, data_len);
    if (HAL_I2C_Mem_Write_IT(&I2cHandle, (uint16_t)dev_address, (uint16_t)mem_address,
                         I2C_MEMADD_SIZE_8BIT, (uint8_t *)TX_buffer, data_len) != HAL_OK) {
        return RET_COM_ERR;
    }

    // if we're supposed to block, wait here
    if (blocking == true) {
        while ( I2C_isBusy() );
    }

    return RET_OK;
}


ret_t I2C_writeByte(uint8_t dev_address, uint8_t mem_address, uint8_t data, bool blocking)
{
    return I2C_writeData(dev_address, mem_address, &data, 1, blocking);
}


ret_t I2C_readData(uint8_t address, uint8_t mem_address, uint8_t * data_ptr, uint8_t data_len)
{
    // check if we're available to send data
    if ( I2C_isBusy() ) {
        return RET_BUSY_ERR;
    }

    // make sure the length is possible
    if (data_len > RX_BUFFER_SIZE) {
        return RET_LEN_ERR;
    }

    // receive that data!
    if (HAL_I2C_Mem_Read_IT(&I2cHandle, (uint16_t)address, (uint16_t)mem_address,
                            I2C_MEMADD_SIZE_8BIT, (uint8_t *)data_ptr, data_len) != HAL_OK ) {
        return RET_COM_ERR;
    }

    // Wait here until the data is read in completely
    while ( I2C_isBusy() );

    return RET_OK;
}


bool I2C_isBusy(void)
{
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(&I2cHandle);
    if ( state == HAL_I2C_STATE_ABORT ||
         state == HAL_I2C_STATE_TIMEOUT ||
         state == HAL_I2C_STATE_ERROR ||
         state == HAL_I2C_STATE_RESET) {
        // major error! (TODO: is reset state that bad?)
        fatal_error_handler(__FILE__, __LINE__, state);
    }
    return (state != HAL_I2C_STATE_READY);
}


/**
  * @brief  Tx Transfer completed callback.
  * @param  I2cHandle: I2C handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    // Can put LED blinkies here for debug. Otherwise, don't need as we have the
    //     I2C_isBusy() function
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    // Can put LED blinkies here for debug. Otherwise, don't need as we have the
    //     I2C_isBusy() function
}

/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
    fatal_error_handler(__FILE__, __LINE__, HAL_I2C_GetError(I2cHandle) );
}
