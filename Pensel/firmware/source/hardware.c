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
#include "hardware.h"
#include "LSM303DLHC.h"

// STM drivers
#include "stm32f3xx.h"
#include "Drivers/stm32f3xx_hal_def.h"
#include "Drivers/stm32f3xx_hal.h"
#include "Drivers/stm32f3xx_hal_rcc.h"
#include "Drivers/stm32f3xx_hal_flash.h"
#include "Drivers/stm32f3xx_hal_gpio.h"
#include "Drivers/stm32f3xx_hal_iwdg.h"
#include "Drivers/stm32f3xx_ll_bus.h"


//! The amount of time, in ms, that must ellapse before changing button / switch state
#define IO_DEBOUNCE_TIMEOUT (10)
#define WDG_COUNT (410u)

IWDG_HandleTypeDef hiwdg;

static GPIO_InitTypeDef  GPIO_InitStruct;

void wdg_clearFlags(void);
void button_ISR(uint16_t GPIO_Pin);
void switch_ISR(uint16_t GPIO_Pin);

// Private IRQ enablers / disablers
inline void mainbtn_irq_enable(void);
inline void mainbtn_irq_disable(void);
inline void auxbtn_irq_enable(void);
inline void auxbtn_irq_disable(void);
inline void switch_irq_enable(void);
inline void switch_irq_disable(void);

inline void sensorDRDY_irq_enable(void);
/* TODO: I don't think I'll ever need this function as DRDY pin interrupt has no possible
    race conditions due to how I queue packets. _I THINK_. Haven't thouroughly looked at
    the code. */
inline void sensorDRDY_irq_disable(void);


//! Tracks a switch's state and manages a switch's hysteresis
typedef struct {
    //! Current state of the button
    switch_state_t state;
    //! If true, the physical state of the pin is different than the current state
    bool changing;
    //! What value the switch is changing to
    volatile switch_state_t changing_value;
    /*! What time the change started. If is more than `IO_DEBOUNCE_TIMEOUT`,
            we will change the current state in `switch_periodic_handler` */
    volatile uint32_t changing_timestamp;
} switch_t;


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
button_t auxbutton_admin;  //!< button handler for the auxilary button.
switch_t switch_admin;     //!< button handler for the 3 position switch.


switch_state_t switch_getval(void) { return switch_admin.state; }
uint8_t mainbutton_getval(void)    { return mainbutton_admin.state; }
uint8_t auxbutton_getval(void)     { return auxbutton_admin.state; }



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

/*! Configures all of the pins (& interrupt priority if applicable) in the project
 *  to what they need to be. (i.e. input, output, falling interrupt, etc)
 */
void configure_pins(void)
{
    switch_state_t switch_state;

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

    // Need to see button going low and high!
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pin   = (BUTTON_MAIN | BUTTON_AUX);
    HAL_GPIO_Init(SWITCH_BUTTON_PORT, &GPIO_InitStruct);

    // switch is active when pin is LOW
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pin   = (SWITCH_0 | SWITCH_1 | SWITCH_2);
    HAL_GPIO_Init(SWITCH_BUTTON_PORT, &GPIO_InitStruct);

    // Sensor input pins
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pin   = (SENSOR_DRDY);  // | SENSOR_INT);
    HAL_GPIO_Init(SENSOR_PORT, &GPIO_InitStruct);

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

    // Thumbwheel time!
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pin  = THUMBWHEEL;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // configure some structures
    mainbutton_admin.state = HAL_GPIO_ReadPin(SWITCH_BUTTON_PORT, BUTTON_MAIN);
    mainbutton_admin.changing = false;
    mainbutton_admin.changing_value = 0;
    mainbutton_admin.changing_timestamp = 0;

    auxbutton_admin.state = HAL_GPIO_ReadPin(SWITCH_BUTTON_PORT, BUTTON_AUX);
    auxbutton_admin.changing = false;
    auxbutton_admin.changing_value = 0;
    auxbutton_admin.changing_timestamp = 0;

    if ( HAL_GPIO_ReadPin(SWITCH_BUTTON_PORT, SWITCH_0) ) {
        switch_state = kSwitch_0;
    } else if ( HAL_GPIO_ReadPin(SWITCH_BUTTON_PORT, SWITCH_1) ) {
        switch_state = kSwitch_1;
    } else if ( HAL_GPIO_ReadPin(SWITCH_BUTTON_PORT, SWITCH_2) ) {
        switch_state = kSwitch_2;
    } else {
        switch_state = kSwitch_0;
    }

    switch_admin.state = switch_state;
    switch_admin.changing = false;
    switch_admin.changing_value = 0;
    switch_admin.changing_timestamp = 0;


    // configure interrupt priority
    HAL_NVIC_SetPriority((IRQn_Type)(EXTI15_10_IRQn), 2, 0); // Sensor DRDY pin

    HAL_NVIC_SetPriority((IRQn_Type)(EXTI1_IRQn), 3, 0);     // Switch pin 0
    HAL_NVIC_SetPriority((IRQn_Type)(EXTI2_TSC_IRQn), 3, 1); // Switch pin 1
    HAL_NVIC_SetPriority((IRQn_Type)(EXTI3_IRQn), 3, 2);     // Switch pin 2

    HAL_NVIC_SetPriority((IRQn_Type)(EXTI9_5_IRQn), 4, 0);   // Main button
    HAL_NVIC_SetPriority((IRQn_Type)(EXTI4_IRQn), 4, 1);     // Aux button

    // enable interrupts
    switch_irq_enable();
    mainbtn_irq_enable();
    auxbtn_irq_enable();
    sensorDRDY_irq_enable();
}

/*! Enables the interrupt corrisponding to the LSM303DLHC DRDY pin. */
inline void sensorDRDY_irq_enable(void)
{
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI15_10_IRQn));
}

/*! Disables the interrupt corrisponding to the LSM303DLHC DRDY pin. */
inline void sensorDRDY_irq_disable(void)
{
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI15_10_IRQn));
}

/*! Enables the interrupt corrisponding to the main button pin. */
inline void mainbtn_irq_enable(void)
{
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn));
}

/*! Disables the interrupt corrisponding to the main button pin. */
inline void mainbtn_irq_disable(void)
{
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI9_5_IRQn));
}

/*! Enables the interrupt corrisponding to the auxilary button pin. */
inline void auxbtn_irq_enable(void)
{
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI4_IRQn));
}

/*! Disables the interrupt corrisponding to the auxilary button pin. */
inline void auxbtn_irq_disable(void)
{
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI4_IRQn));
}

/*! Enables the interrupts corrisponding to the 3 position switch pins. */
inline void switch_irq_enable(void)
{
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI1_IRQn));
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI2_TSC_IRQn));
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI3_IRQn));
}

/*! Disables the interrupts corrisponding to the 3 position switch pins. */
inline void switch_irq_disable(void)
{
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI1_IRQn));
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI2_TSC_IRQn));
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI3_IRQn));
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

    // next, aux button
    auxbtn_irq_disable();
    if (auxbutton_admin.changing == true) {
        if (current_tick - auxbutton_admin.changing_timestamp >= IO_DEBOUNCE_TIMEOUT) {
            auxbutton_admin.state = auxbutton_admin.changing_value;
            auxbutton_admin.changing = false;
        }
    }
    auxbtn_irq_enable();
}


/*! Gets called periodically and handles if we should update the switch
 *  state based on if it has gotten out of the debounce timeout.
 *
 * @param[in] current_tick (uint32_t): Current system tick in ms
 */
void switch_periodic_handler(uint32_t current_tick)
{
    switch_irq_disable();
    if (switch_admin.changing == true) {
        if (current_tick - switch_admin.changing_timestamp >= IO_DEBOUNCE_TIMEOUT) {
            switch_admin.state = switch_admin.changing_value;
            switch_admin.changing = false;
        }
    }
    switch_irq_enable();
}


/*! gets called if either button has triggered the HW falling / rising interrupt.
 *  We then update the button structs accordingly to indicate if we're changing states.
 *
 * @param[in] GPIO_Pin (uint16_t): Which pin triggered the interrupt.
 */
void button_ISR(uint16_t GPIO_Pin)
{
    uint8_t pin_val = HAL_GPIO_ReadPin(SWITCH_BUTTON_PORT, GPIO_Pin);

    if (GPIO_Pin == BUTTON_MAIN) {
        if (mainbutton_admin.state != pin_val) {
            mainbutton_admin.changing_value = pin_val;
            mainbutton_admin.changing_timestamp = HAL_GetTick();
            mainbutton_admin.changing = true;
        } else {
            mainbutton_admin.changing = false;
        }
    } else {
        if (auxbutton_admin.state != pin_val) {
            auxbutton_admin.changing_value = pin_val;
            auxbutton_admin.changing_timestamp = HAL_GetTick();
            auxbutton_admin.changing = true;
        } else {
            auxbutton_admin.changing = false;
        }
    }
}


/*! gets called if any of the switch pins have triggered the HW falling interrupt.
 *  We then update the switch struct accordingly to indicate if we're changing states.
 *
 * @param[in] GPIO_Pin (uint16_t): Which pin triggered the interrupt.
 */
void switch_ISR(uint16_t GPIO_Pin)
{
    switch_state_t new_state = switch_admin.state;

    // Figure out which pin caused the interrupt
    switch(GPIO_Pin) {
        case SWITCH_0:
            new_state = kSwitch_0;
            break;

        case SWITCH_1:
            new_state = kSwitch_1;
            break;

        case SWITCH_2:
            new_state = kSwitch_2;
            break;
    }

    if (switch_admin.state != new_state) {
        switch_admin.changing_value = new_state;
        switch_admin.changing_timestamp = HAL_GetTick();
        switch_admin.changing = true;
    } else {
        switch_admin.changing = false;
    }
}


/* independent watchdog code */

/*! Initializes the independent watchdog module to require a pet every 10 ms
 *
 * @return success or failure of initializing the watchdog module
 */
ret_t wdg_init(void)
{
    // the LSI counter used for wdg timer is @41KHz.
    // 10 ms counter window count: counter_val = (10 ms) * (41 kHz) ~= 410
    hiwdg.Instance = IWDG;
    //! Select the prescaler of the IWDG. This parameter can be a value of @ref IWDG_Prescaler
    hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
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


/*! Figure out which pin triggered the interrupt and call the corrisponding
 *  interrupt handler.
 *
 * @param[in] GPIO_Pin (uint16_t): Which pin triggered the interrupt.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case BUTTON_MAIN:
        case BUTTON_AUX:
            button_ISR(GPIO_Pin);
            break;

        case SWITCH_0:
        case SWITCH_1:
        case SWITCH_2:
            switch_ISR(GPIO_Pin);
            break;
        case SENSOR_DRDY:
            LSM303DLHC_drdy_ISR();
            break;
        // case SENSOR_INT:
        //     LSM303DLHC_int_handler();
        //     break;
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
    if (value == GPIO_PIN_RESET || value == GPIO_PIN_SET) {
        if (LED == LED_0) {
            HAL_GPIO_WritePin(LED_PORT, LED_0, value);
        } else if (LED == LED_1) {
            HAL_GPIO_WritePin(LED_PORT, LED_1, value);
        }
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
