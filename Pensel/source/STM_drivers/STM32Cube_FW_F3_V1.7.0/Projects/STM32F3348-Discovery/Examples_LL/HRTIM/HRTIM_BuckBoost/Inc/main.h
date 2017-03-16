/**
  ******************************************************************************
  * @file    Examples_LL/HRTIM/HRTIM_BuckBoost/Inc/main.h
  * @author  MCD Application Team
  * @version V1.7.0
  * @date    16-December-2016
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_exti.h"
#include "stm32f3xx_ll_pwr.h"
#include "stm32f3xx_ll_cortex.h"
#include "stm32f3xx_ll_adc.h"
#include "stm32f3xx_ll_hrtim.h"
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Exported types ------------------------------------------------------------*/
typedef enum {
  LED3 = 0,
  LED4 = 1,
  LED5 = 2,
  LED6 = 3,

  LED_GREEN  = LED5,
  LED_ORANGE = LED4, 
  LED_RED    = LED3, 
  LED_BLUE   = LED6
} Led_t;

typedef enum {
  DEMO_STATE_BUCK = 0,
  DEMO_STATE_BOOST,
  DEMO_STATE_DE_ENERGIZE,
  DEMO_STATE_FAULT
} DemoState_t;

/* Exported constants --------------------------------------------------------*/
/**
  * @brief LED3 
  */
#define LED3_PIN               LL_GPIO_PIN_6
#define LED3_GPIO_PORT         GPIOB
#define LED3_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)  

/**
  * @brief LED4 
  */
#define LED4_PIN               LL_GPIO_PIN_8
#define LED4_GPIO_PORT         GPIOB
#define LED4_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)  

/**
  * @brief LED5 
  */
#define LED5_PIN               LL_GPIO_PIN_9
#define LED5_GPIO_PORT         GPIOB
#define LED5_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)  

/**
  * @brief LED6 
  */
#define LED6_PIN               LL_GPIO_PIN_7
#define LED6_GPIO_PORT         GPIOB
#define LED6_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)  

/**
  * @brief User push-button
  */
#define USER_BUTTON_PIN               LL_GPIO_PIN_0
#define USER_BUTTON_GPIO_PORT         GPIOA
#define USER_BUTTON_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)

/**
  * @brief ADC1_IN2 
  */
#define ADC1_IN2_PIN               LL_GPIO_PIN_1
#define ADC1_IN2_GPIO_PORT         GPIOA
#define ADC1_IN2_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)  

/**
  * @brief ADC1_IN4 
  */
#define ADC1_IN4_PIN               LL_GPIO_PIN_3
#define ADC1_IN4_GPIO_PORT         GPIOA
#define ADC1_IN4_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA) 

/**
  * @brief HRTIM1_CHA1 
  */
#define HRTIM1_CHA1_PIN               LL_GPIO_PIN_8
#define HRTIM1_CHA1_GPIO_PORT         GPIOA
#define HRTIM1_CHA1_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA) 

/**
  * @brief HRTIM1_CHA2 
  */
#define HRTIM1_CHA2_PIN               LL_GPIO_PIN_9
#define HRTIM1_CHA2_GPIO_PORT         GPIOA
#define HRTIM1_CHA2_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)  

/**
  * @brief HRTIM1_CHB1 
  */
#define HRTIM1_CHB1_PIN               LL_GPIO_PIN_10
#define HRTIM1_CHB1_GPIO_PORT         GPIOA
#define HRTIM1_CHB1_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)  

/**
  * @brief HRTIM1_CHB2 
  */
#define HRTIM1_CHB2_PIN               LL_GPIO_PIN_11
#define HRTIM1_CHB2_GPIO_PORT         GPIOA
#define HRTIM1_CHB2_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)  

/**
  * @brief HRTIM1_FLT1 
  */
#define HRTIM1_FLT1_PIN               LL_GPIO_PIN_12
#define HRTIM1_FLT1_GPIO_PORT         GPIOA
#define HRTIM1_FLT1_GPIO_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)  

/**
  * @brief Toggle periods for various blinking modes
  */
#define LED_BLINK_FAST  200
#define LED_BLINK_SLOW  500
#define LED_BLINK_ERROR 1000


#define BUCK_PWM_PERIOD ((uint16_t)18432) // 250kHz

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern DemoState_t DemoState;


/* Exported functions ------------------------------------------------------- */
extern void LED_Init(Led_t Led);
extern void LED_On(Led_t Led);
extern void LED_Off(Led_t Led);
extern void LED_Toggle(Led_t Led);
extern void LED_Toggle(Led_t Led);
extern void LED_Blinking(uint32_t Period);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
