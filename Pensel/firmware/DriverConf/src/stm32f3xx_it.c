/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @author  MCD Application Team
  * @version V1.7.0
  * @date    16-December-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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

#include "hardware.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"

// Peripherals used
#include "Drivers/stm32f3xx_hal_i2c.h"
#include "Drivers/stm32f3xx_hal_uart.h"


/* I2C handler declared in "main.c" file */
extern I2C_HandleTypeDef I2cHandle;
extern UART_HandleTypeDef HAL_UART_handle;

/******************************************************************************
 *            Cortex-M4 Processor Exceptions Handlers                         *
 ******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1);
}

/**
  * @brief  This function handles Memory Manage exception.
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1);
}

/**
  * @brief  This function handles Bus Fault exception.
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1);
}

/**
  * @brief  This function handles Usage Fault exception.
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1);
}

/**
  * @brief  This function handles SVCall exception.
  */
void SVC_Handler(void)
{
    while (1);
}

/**
  * @brief  This function handles Debug Monitor exception.
  */
void DebugMon_Handler(void)
{
    while (1);
}

/**
  * @brief  This function handles PendSVC exception.
  */
void PendSV_Handler(void)
{
    while (1);
}

/**
  * @brief  This function handles SysTick Handler.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************
 *                 STM32F3xx Peripherals Interrupt Handlers                    *
 *  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the   *
 *  available peripheral interrupt handler's name please refer to the startup  *
 *  file (startup_stm32f3xx.s).                                                *
 ******************************************************************************/

/**
  * @brief  This function handles I2C event interrupt request.
  */
void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&I2cHandle);
}

/**
  * @brief  This function handles I2C error interrupt request.
  */
void I2C2_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&I2cHandle);
}

/**
  * @brief  This function handles UART interrupt request.
  */
void USART1_IRQHandler(void)
{
    // LED_toggle(LED_1);
    HAL_UART_IRQHandler(&HAL_UART_handle);
}

/**
  * @brief  This function handles the pin 1 external interrupt.
  */
void EXTI1_IRQHandler(void)
{
    // Have the HAL lib cleanup and call our callback
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/**
  * @brief  This function handles the pin 2 external interrupt.
  */
void EXTI2_TSC_IRQHandler(void)
{
    // Have the HAL lib cleanup and call our callback
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

/**
  * @brief  This function handles the pin 3 external interrupt.
  */
void EXTI3_IRQHandler(void)
{
    // Have the HAL lib cleanup and call our callback
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

/**
  * @brief  This function handles the pin 4 external interrupt.
  */
void EXTI4_IRQHandler(void)
{
    // Have the HAL lib cleanup and call our callback
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

/**
  * @brief  This function handles the pins 5 thru 9 external interrupt.
  */
void EXTI9_5_IRQHandler(void)
{
    // (pin 5 is the only one configured for interrupt)
    // Have the HAL lib cleanup and call our callback
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

/**
  * @brief  This function handles the pins 10 thru 15 external interrupt.
  */
void EXTI15_10_IRQHandler(void)
{
    // Only pin 11 is congfigured for interrupt
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
