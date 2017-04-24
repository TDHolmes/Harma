/*
 *
 */

// User includes
#include "common.h"
#include "hardware.h"

// STM drivers
#include "stm32f3xx.h"
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"
#include "Drivers/stm32f3xx_hal_rcc.h"
#include "Drivers/stm32f3xx_hal_flash.h"
#include "Drivers/stm32f3xx_hal_gpio.h"
#include "Drivers/stm32f3xx_ll_bus.h"


static GPIO_InitTypeDef  GPIO_InitStruct;


void button_handler(uint16_t GPIO_Pin);
void switch_handler(uint16_t GPIO_Pin);


// TODO: don't do this
void button_handler(uint16_t GPIO_Pin)
{
    fatal_error_handler();
}

// TODO: don't do this
void switch_handler(uint16_t GPIO_Pin)
{
    fatal_error_handler();
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV                     = 1
  *            PLLMUL                         = RCC_PLL_MUL9 (9)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        /* Initialization Error */
        fatal_error_handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
    clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        /* Initialization Error */
        fatal_error_handler();
    }
}


void configure_pins(void)
{
    // configure all pins for the Pensel project
    // Turn on the clocks to the GPIO peripherals
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);

    // Push-Pull outputs for the LEDs!
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin   = (LED_0 | LED_1);

    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // Switches and buttons
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pin   = (BUTTON_MAIN | BUTTON_AUX | SWITCH_0 | SWITCH_1 | SWITCH_2);
    HAL_GPIO_Init(SWITCH_BUTTON_PORT, &GPIO_InitStruct);

    NVIC_SetPriority((IRQn_Type)(EXTI1_IRQn), 3);  // Low priority
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI1_IRQn));

    NVIC_SetPriority((IRQn_Type)(EXTI2_TSC_IRQn), 3);  // Low priority
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI2_TSC_IRQn));

    NVIC_SetPriority((IRQn_Type)(EXTI3_IRQn), 3);  // Low priority
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI3_IRQn));

    NVIC_SetPriority((IRQn_Type)(EXTI4_IRQn), 3);  // Low priority
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI4_IRQn));

    NVIC_SetPriority((IRQn_Type)(EXTI9_5_IRQn), 3);  // Low priority
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn));

    // Sensor input pins
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pin   = (SENSOR_DRDY | SENSOR_INT);
    HAL_GPIO_Init(SENSOR_PORT, &GPIO_InitStruct);

    // I2C time!
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD; // alternate function - Open Drain
    GPIO_InitStruct.Pin   = (I2C_SCL | I2C_SDA);
    HAL_GPIO_Init(I2C_PORT, &GPIO_InitStruct);

    // UART time!
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP; // alternate function - Open Drain
    GPIO_InitStruct.Pin   = (UART_RX | UART_TX);
    HAL_GPIO_Init(UART_PORT, &GPIO_InitStruct);

    // Thumbwheel time!
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pin  = THUMBWHEEL;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
}


// TODO: implement a good callback...
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case BUTTON_MAIN:
        case BUTTON_AUX:
            button_handler(GPIO_Pin);
            break;

        case SWITCH_0:
        case SWITCH_1:
        case SWITCH_2:
            switch_handler(GPIO_Pin);
            break;
    }
}


void LED_toggle(uint16_t LED)
{
    if (LED == LED_0) {
        HAL_GPIO_TogglePin(LED_PORT, LED_0);
    } else if (LED == LED_1) {
        HAL_GPIO_TogglePin(LED_PORT, LED_1);
    }
}


void LED_set(uint16_t LED, uint8_t value)
{
    if (value == GPIO_PIN_RESET || value == GPIO_PIN_SET) {
        if (LED == LED_0) {
            HAL_GPIO_WritePin(LED_PORT, LED_0, value);
        } else if (LED == LED_1) {
            HAL_GPIO_WritePin(LED_PORT, LED_1, value);
        }
    }
}


// GPIO_PinState     HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
// void              HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
// void              HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
