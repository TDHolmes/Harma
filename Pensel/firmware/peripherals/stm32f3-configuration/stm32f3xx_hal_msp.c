/**
  ******************************************************************************
  * @file    stm32f3xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.7.0
  * @date    16-December-2016
  * @brief   HAL MSP module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "stm32f3xx.h"
#include "peripherals/stm32f3/stm32f3xx_hal_def.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"
#include "peripherals/stm32f3/stm32f3xx_hal_uart.h"
#include "peripherals/stm32f3/stm32f3xx_hal_i2c.h"
// Reset/clocking stuff I believe
#include "peripherals/stm32f3/stm32f3xx_hal_rcc.h"
#include "peripherals/stm32f3/stm32f3xx_hal_rcc_ex.h"
// USB stuff
#include "peripherals/stm32f3/stm32f3xx_hal_pcd.h"
#include "peripherals/stm32f3/stm32f3xx_hal_pcd_ex.h"

// I2C definitions!!!
#define I2Cx                            I2C1
#define RCC_PERIPHCLK_I2Cx              RCC_PERIPHCLK_I2C1
#define RCC_I2CxCLKSOURCE_SYSCLK        RCC_I2C1CLKSOURCE_SYSCLK
#define I2Cx_CLK_ENABLE()               __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()


/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SDA_PIN                    GPIO_PIN_7
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SCL_SDA_AF                 GPIO_AF4_I2C1

#define I2Cx_FORCE_RESET()              __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()            __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx's NVIC */
#define I2Cx_EV_IRQn                    I2C1_EV_IRQn
#define I2Cx_ER_IRQn                    I2C1_ER_IRQn
#define I2Cx_EV_IRQHandler              I2C1_EV_IRQHandler
#define I2Cx_ER_IRQHandler              I2C1_ER_IRQHandler

// USART definitions!!!
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
// #define USARTx_IRQHandler                USART1_IRQHandler


// ADC crap! :D
/* Definition for ADCx clock resources */
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

#define DMAx_CHANNELx_CLK_ENABLE()      __HAL_RCC_DMA1_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC1_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC1_RELEASE_RESET()


/* Private functions ---------------------------------------------------------*/

/**
  * @brief I2C MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
    if(hi2c->Instance == I2C1)
    {
        /*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
        RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2Cx;
        RCC_PeriphCLKInitStruct.I2c2ClockSelection = RCC_I2CxCLKSOURCE_SYSCLK;
        HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

        /*##-2- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        I2Cx_SCL_GPIO_CLK_ENABLE();
        I2Cx_SDA_GPIO_CLK_ENABLE();
        /* Enable I2Cx clock */
        I2Cx_CLK_ENABLE();

        /*##-3- Configure peripheral GPIO ##########################################*/
        /* I2C TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
        HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

        /* I2C RX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = I2Cx_SDA_PIN;
        GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
        HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

        /*##-4- Configure the NVIC for I2C ########################################*/
        /* NVIC for I2Cx */
        HAL_NVIC_SetPriority(I2Cx_ER_IRQn, 0, 0);  // Highest Preempt Priority, highest Sub Priority
        HAL_NVIC_EnableIRQ(I2Cx_ER_IRQn);
        HAL_NVIC_SetPriority(I2Cx_EV_IRQn, 0, 1);  // Highest Preempt Priority, 2nd highest Sub Priority
        HAL_NVIC_EnableIRQ(I2Cx_EV_IRQn);
    } else {
        fatal_error_handler(__FILE__, __LINE__, -1);
    }
}

/**
  * @brief I2C MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    if(hi2c->Instance == I2C1)
    {
        /*##-1- Reset peripherals ##################################################*/
        I2Cx_FORCE_RESET();
        I2Cx_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks #################################*/
        /* Configure I2C Tx as alternate function  */
        HAL_GPIO_DeInit(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN);
        /* Configure I2C Rx as alternate function  */
        HAL_GPIO_DeInit(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN);

        /*##-3- Disable the NVIC for I2C ##########################################*/
        HAL_NVIC_DisableIRQ(I2Cx_ER_IRQn);
        HAL_NVIC_DisableIRQ(I2Cx_EV_IRQn);
    } else {
        fatal_error_handler(__FILE__, __LINE__, -1);
    }
}


/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    if(huart->Instance == USART1)
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        USARTx_TX_GPIO_CLK_ENABLE();
        USARTx_RX_GPIO_CLK_ENABLE();


        /* Enable USARTx clock */
        USARTx_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = USARTx_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = USARTx_TX_AF;

        HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USARTx_RX_PIN;
        GPIO_InitStruct.Alternate = USARTx_RX_AF;

        HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC for UART ########################################*/
        /* NVIC for USART */
        HAL_NVIC_SetPriority(USARTx_IRQn, 1, 0);  // 2nd Highest Preempt Priority, 3rd highest Sub Priority
        HAL_NVIC_EnableIRQ(USARTx_IRQn);
    } else {
        fatal_error_handler(__FILE__, __LINE__, -1);
    }
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        /*##-1- Reset peripherals ##################################################*/
        USARTx_FORCE_RESET();
        USARTx_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks #################################*/
        /* Configure UART Tx as alternate function  */
        HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
        /* Configure UART Rx as alternate function  */
        HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

        /*##-3- Disable the NVIC for UART ##########################################*/
        HAL_NVIC_DisableIRQ(USARTx_IRQn);
    } else {
        fatal_error_handler(__FILE__, __LINE__, -1);
    }
}


void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
    if(hpcd->Instance == USB) {
        /* Peripheral clock enable */
        __HAL_RCC_USB_CLK_ENABLE();
        /* USB interrupt Init */
        HAL_NVIC_SetPriority(USB_HP_CAN_TX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USB_HP_CAN_TX_IRQn);
        HAL_NVIC_SetPriority(USB_LP_CAN_RX0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
    } else {
        fatal_error_handler(__FILE__, __LINE__, -1);
    }
}


void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd)
{

    if(hpcd->Instance == USB) {
        /* Peripheral clock disable */
        __HAL_RCC_USB_CLK_DISABLE();

        /* USB interrupt DeInit */
        HAL_NVIC_DisableIRQ(USB_HP_CAN_TX_IRQn);
        HAL_NVIC_DisableIRQ(USB_LP_CAN_RX0_IRQn);
    } else {
        fatal_error_handler(__FILE__, __LINE__, -1);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
