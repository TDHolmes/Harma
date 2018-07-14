/*!
 * @file    hardware.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Hardware specific definitions and function calls.
 *
 */

#include <stdint.h>
#include <stdbool.h>

// User includes
#include "common.h"
// #include "modules/LSM303DLHC/LSM303DLHC.h"
#include "modules/LSM9DS1/LSM9DS1.h"

// STM drivers
#include "peripherals/stm32f3-configuration/stm32f3xx.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"
#include "peripherals/stm32f3/stm32f3xx_hal_def.h"
#include "peripherals/stm32f3/stm32f3xx_hal_rcc.h"
#include "peripherals/stm32f3/stm32f3xx_hal_flash.h"
#include "peripherals/stm32f3/stm32f3xx_hal_gpio.h"
#include "peripherals/stm32f3/stm32f3xx_hal_iwdg.h"
#include "peripherals/stm32f3/stm32f3xx_ll_bus.h"

#include "hardware.h"


//! The amount of time, in ms, that must ellapse before changing button / switch state
#define IO_DEBOUNCE_TIMEOUT (10)

static GPIO_InitTypeDef  GPIO_InitStruct;

#ifdef WATCHDOG_ENABLE

    #define WDG_COUNT (161u)
    IWDG_HandleTypeDef hiwdg;
    void wdg_clearFlags(void);

#endif

void button_ISR(uint16_t GPIO_Pin);

// Private IRQ enablers / disablers
static inline void mainbtn_irq_enable(void);
static inline void mainbtn_irq_disable(void);


//! Tracks a button's state and manages a button's hysteresis
typedef struct {
    //! current state of the button (1 or 0)
    uint8_t state;
    //! If true, the physical state of the pin is different than the current state
    bool changing;
    //! What value the switch is changing to
    volatile uint8_t changing_value;
    /*! What time the change started. If is more than `IO_DEBOUNCE_TIMEOUT`,
        we will change the current state in `button_periodic_handler` */
    volatile uint32_t changing_timestamp;
} button_t;


button_t mainbutton_admin; //!< button handler for the main button.

uint8_t mainbutton_getval(void)    { return mainbutton_admin.state; }


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 64000000
  *            HCLK(Hz)                       = 64000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            PLLMUL                         = RCC_PLL_MUL16 (16)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */

#ifdef PENSEL_V1
    void SystemClock_Config(void)
    {
        RCC_ClkInitTypeDef RCC_ClkInitStruct;
        RCC_OscInitTypeDef RCC_OscInitStruct;
        HAL_StatusTypeDef retval;

        /* HSI Oscillator already ON after system reset, activate PLL with HSI as source */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
        retval = HAL_RCC_OscConfig(&RCC_OscInitStruct);
        if (retval != HAL_OK)
        {
            /* Initialization Error */
            fatal_error_handler(__FILE__, __LINE__, retval);
        }

        /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
         clocks dividers */
        RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
        retval = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
        if (retval != HAL_OK)
        {
            /* Initialization Error */
            fatal_error_handler(__FILE__, __LINE__, retval);
        }
    }
#elif PENSEL_V2 || PENSEL_UNITTESTS
    void SystemClock_Config(void)
    {
        RCC_OscInitTypeDef RCC_OscInitStruct;
        RCC_ClkInitTypeDef RCC_ClkInitStruct;
        RCC_PeriphCLKInitTypeDef PeriphClkInit;
        HAL_StatusTypeDef retval;

        /* Initializes the CPU, AHB and APB busses clock */
        RCC_OscInitStruct.OscillatorType =
            RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV4;
        RCC_OscInitStruct.HSIState = RCC_HSI_ON;
        RCC_OscInitStruct.HSICalibrationValue = 16;
        RCC_OscInitStruct.LSIState = RCC_LSI_ON;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
        retval = HAL_RCC_OscConfig(&RCC_OscInitStruct);
        if (retval != HAL_OK)
        {
              fatal_error_handler(__FILE__, __LINE__, retval);
        }

        /* Initializes the CPU, AHB and APB busses clocks */
        RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

        retval = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
        if (retval != HAL_OK)
        {
              fatal_error_handler(__FILE__, __LINE__, retval);
        }

        PeriphClkInit.PeriphClockSelection =
            RCC_PERIPHCLK_USB | RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1;
        PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
        PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
        PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
        retval = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
        if (retval != HAL_OK)
        {
            fatal_error_handler(__FILE__, __LINE__, retval);
        }

          /**Configure the Systick interrupt time
          */
        HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

          /**Configure the Systick
          */
        HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

        /* SysTick_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    }
#endif


void hw_setUSBpullup(uint8_t value)
{
    if (value) {
        HAL_GPIO_WritePin(USB_PORT, USB_DP_PULLUP, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(USB_PORT, USB_DP_PULLUP, GPIO_PIN_RESET);
    }
}


/*! Configures all of the pins (& interrupt priority if applicable) in the project
 *  to what they need to be. (i.e. input, output, falling interrupt, etc)
 */
void configure_pins(void)
{
    // configure all pins for the Pensel project
    // Turn on the clocks to the GPIO peripherals
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    // Push-Pull outputs for the LEDs and timing pin!
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin   = (LED_0 | LED_1 | EXTRA_GPIO);

    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // Need to see button going low and high!
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pin   = BUTTON_MAIN;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);

    // Push-Pull outputs for the LEDs and timing pin!
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin   = USB_DP_PULLUP;

    HAL_GPIO_Init(USB_PORT, &GPIO_InitStruct);

    // Sensor input pins
    // TODO: re-enable for sensor
    // GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    // GPIO_InitStruct.Pin   = (SENSOR_DRDY);  // | SENSOR_INT);
    // HAL_GPIO_Init(SENSOR_PORT, &GPIO_InitStruct);

    // These are configured in stm32f3xx_hal_msp.c
    // // I2C time!
    // GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD; // alternate function - Open Drain
    // GPIO_InitStruct.Pin   = (I2C_SCL | I2C_SDA);
    // HAL_GPIO_Init(I2C_PORT, &GPIO_InitStruct);
    //
    // // UART time!
    // GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP; // alternate function - Open Drain
    // GPIO_InitStruct.Pin   = (UART_RX | UART_TX);
    // HAL_GPIO_Init(UART_PORT, &GPIO_InitStruct);

    // configure some structures
    mainbutton_admin.state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_MAIN);
    mainbutton_admin.changing = false;
    mainbutton_admin.changing_value = 0;
    mainbutton_admin.changing_timestamp = 0;

    // configure interrupt priority
    // TODO: fill in interrupt priorities for the sensors
    // HAL_NVIC_SetPriority((IRQn_Type)(EXTI15_10_IRQn), 2, 0); // Sensor DRDY pin
    //
    // HAL_NVIC_SetPriority((IRQn_Type)(EXTI1_IRQn), 3, 0);     // Switch pin 0
    // HAL_NVIC_SetPriority((IRQn_Type)(EXTI2_TSC_IRQn), 3, 1); // Switch pin 1

    HAL_NVIC_SetPriority((IRQn_Type)(EXTI3_IRQn), 4, 0);     // Main button
    // HAL_NVIC_SetPriority((IRQn_Type)(EXTI4_IRQn), 4, 1);     // Aux button

    // enable interrupts
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI4_IRQn));  // make sure Aux button IRQ is disabled
    mainbtn_irq_enable();
}

/*! Enables the interrupt corrisponding to the main button pin. */
static inline void mainbtn_irq_enable(void)
{
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn));
}

/*! Disables the interrupt corrisponding to the main button pin. */
static inline void mainbtn_irq_disable(void)
{
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI9_5_IRQn));
}


/*! Gets called periodically and handles if we should update the either of the
 *  buttons state based on if it has gotten out of the debounce timeout.
 *
 * @param[in] current_tick (uint32_t): Current system tick in ms
 */
void button_periodic_handler(uint32_t current_tick)
{
    // first, handle the main button
    mainbtn_irq_disable();
    if (mainbutton_admin.changing == true) {
        if (current_tick - mainbutton_admin.changing_timestamp >= IO_DEBOUNCE_TIMEOUT) {
            mainbutton_admin.state = mainbutton_admin.changing_value;
            mainbutton_admin.changing = false;
        }
    }
    mainbtn_irq_enable();
}



/*! gets called if either button has triggered the HW falling / rising interrupt.
 *  We then update the button structs accordingly to indicate if we're changing states.
 *
 * @param[in] GPIO_Pin (uint16_t): Which pin triggered the interrupt.
 */
void button_ISR(uint16_t GPIO_Pin)
{
    uint8_t pin_val = HAL_GPIO_ReadPin(BUTTON_PORT, GPIO_Pin);

    if (GPIO_Pin == BUTTON_MAIN) {
        if (mainbutton_admin.state != pin_val) {
            mainbutton_admin.changing_value = pin_val;
            mainbutton_admin.changing_timestamp = HAL_GetTick();
            mainbutton_admin.changing = true;
        } else {
            mainbutton_admin.changing = false;
        }
    }
}


/* independent watchdog code */
#ifdef WATCHDOG_ENABLE

    /*! Initializes the independent watchdog module to require a pet every 10 ms
     *
     * @return success or failure of initializing the watchdog module
     */
    ret_t wdg_init(void)
    {
        // the LSI counter used for wdg timer is @41KHz.
        // shooting for a 1 second window. 1 / ((1 / 41 kHz) * 256) ~= 160.15 = 161
        hiwdg.Instance = IWDG;
        //! Select the prescaler of the IWDG. This parameter can be a value of @ref IWDG_Prescaler
        hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
        /*! Specifies the IWDG down-counter reload value. This parameter must
          be a number between Min_Data = 0 and Max_Data = 0x0FFFU */
        hiwdg.Init.Reload = WDG_COUNT;
        /*!< Specifies the window value to be compared to the down-counter.
            This parameter must be a number between Min_Data = 0 and Max_Data = 0x0FFFU */
        hiwdg.Init.Window = WDG_COUNT;

        if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
            return RET_GEN_ERR;
        }
        __HAL_DBGMCU_FREEZE_IWDG();
        return RET_OK;
    }

    /*! Resets the watchdog timer (pets it) so we don't reset
     *
     * @return success or failure of petting the watchdog
     */
    ret_t wdg_pet(void)
    {
        if (HAL_IWDG_Refresh(&hiwdg) == HAL_OK) {
            return RET_OK;
        } else {
            return RET_GEN_ERR;
        }
    }

    /*! Checks if the watchdog was the reason for our reset
     *
     * @return true if watchdog was the reason for the reset
     */
    bool wdg_isSet(void)
    {
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == RESET) {
            wdg_clearFlags();
            return false;
        } else {
            wdg_clearFlags();
            return true;
        }
    }

    /*! Private method to reset the reset flags on boot
     */
    void wdg_clearFlags(void)
    {
        // Clear reset flags
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }
#endif

/* USB init function */
void hw_USB_init(void)
{
    PCD_HandleTypeDef hpcd_USB_FS;
    HAL_StatusTypeDef ret;

    hpcd_USB_FS.Instance = USB;
    hpcd_USB_FS.Init.dev_endpoints = 8;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.ep0_mps = DEP0CTL_MPS_64;
    hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;

    ret = HAL_PCD_Init(&hpcd_USB_FS);
    if (ret != HAL_OK) {
        check_retval_fatal(__FILE__, __LINE__, (ret_t)ret);
    }
}


/*! Figure out which pin triggered the interrupt and call the corrisponding
 *  interrupt handler.
 *
 * @param[in] GPIO_Pin (uint16_t): Which pin triggered the interrupt.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case BUTTON_MAIN:
            button_ISR(GPIO_Pin);
            break;

        case SENSOR_INT1:
            LSM9DS1_INT1_ISR();
            break;

        case SENSOR_INT2:
            LSM9DS1_INT2_ISR();
            break;

        case SENSOR_DRDY:
            LSM9DS1_DRDY_ISR();
            break;

        // TODO: fill in ISRs for the sensor

        default:
            // TODO: log error here?
            break;
    }
}


/*! Toggles the given LED.
 *
 * @param[in] LED (uint16_t): Which LED to toggle.
 */
void LED_toggle(uint16_t LED)
{
    if (LED == LED_0) {
        HAL_GPIO_TogglePin(LED_PORT, LED_0);
    } else if (LED == LED_1) {
        HAL_GPIO_TogglePin(LED_PORT, LED_1);
    }
}


/*! Sets the given LED to the given value.
 *
 * @param[in] LED (uint16_t): Which LED to sdt.
 * @param[in] value (uint16_t): what to set the LED to.
 */
void LED_set(uint16_t LED, uint8_t value)
{
    // LEDs are active low config...
    if (value == GPIO_PIN_RESET) {
        value = GPIO_PIN_SET;
    } else if(value == GPIO_PIN_SET) {
        value = GPIO_PIN_RESET;
    } else {
        return;  // invalid value!!
    }

    // Write the value to the LED...
    if (LED == LED_0) {
        HAL_GPIO_WritePin(LED_PORT, LED_0, value);
    } else if (LED == LED_1) {
        HAL_GPIO_WritePin(LED_PORT, LED_1, value);
    }
}

/*! Toggles the extra pin used for timing.
 *
 */
void TimingPin_toggle(void)
{
    HAL_GPIO_TogglePin(LED_PORT, EXTRA_GPIO);
}

/*! Sets the extra pin used for timing to the given value.
 *
 * @param[in] value (uint8_t): what to set the LED to.
 */
void TimingPin_set(uint8_t value)
{
    HAL_GPIO_WritePin(LED_PORT, EXTRA_GPIO, value);
}


// GPIO_PinState     HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
// void              HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
// void              HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
