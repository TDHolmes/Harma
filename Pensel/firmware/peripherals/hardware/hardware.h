/*!
 * @file    hardware.h
 * @author  Tyler Holmes
 *
 * @date    20-May-2017
 * @brief   Hardware specific definitions and function calls.
 */
#pragma once

#include "common.h"
#include "peripherals/stm32f3-configuration/stm32f3xx.h"
#include "peripherals/stm32f3/stm32f3xx_hal_gpio.h"
#include <stdbool.h>
#include <stdint.h>

// Sensor stuff
#define SENSOR_PORT (GPIOA)
#define SENSOR_DRDY (GPIO_PIN_2)
#define SENSOR_INT1 (GPIO_PIN_0)
#define SENSOR_INT2 (GPIO_PIN_1)

// LED / exposed pad stuff
#define LED_PORT (GPIOA)
#define LED_0 (GPIO_PIN_6)
#define LED_1 (GPIO_PIN_7)
#define EXTRA_GPIO (GPIO_PIN_8)

// Button & switch stuff
#define BUTTON_PORT (GPIOA)
// #define BUTTON_AUX   (GPIO_PIN_4)
#define BUTTON_MAIN (GPIO_PIN_3)

// I2C
#define I2C_PORT (GPIOB)
#define I2C_SDA (GPIO_PIN_7)
#define I2C_SCL (GPIO_PIN_6)

// UART
#define UART_PORT (GPIOA)
#define UART_RX (GPIO_PIN_10)
#define UART_TX (GPIO_PIN_9)

// USB
#define USB_PORT (GPIOA)
#define USB_DM (GPIO_PIN_11)
#define USB_DP (GPIO_PIN_12)
#define USB_DP_PULLUP (GPIO_PIN_4)

// Board rev
#define BREV_PORT (GPIOB)
#define BREV_0 (GPIO_PIN_4)
#define BREV_1 (GPIO_PIN_5)

//! ALL interrupt prioritizations! (in order)
// 0 - systick must preempt other interrupts to keep acurate time
#define SysTick_INT_PREEMPT_PRI (0)
#define SysTick_INT_SUB_PRI (0)

// 1 - USB stuff...
#define USBTX_INT_PREMPT_PRI (3)
#define USBTX_INT_SUB_PRI (0)
#define USBRX_INT_PREMPT_PRI (3)
#define USBRX_INT_SUB_PRI (1)

// 2 - Communications...
#define I2C_ER_INT_PREEMPT_PRI (2)
#define I2C_ER_INT_SUB_PRI (0)
#define I2C_EV_INT_PREEMPT_PRI (2)
#define I2C_EV_INT_SUB_PRI (1)
#define USART_INT_PREMPT_PRI (2)
#define USART_INT_SUB_PRI (2)

// 3 - External pin interrupts
#define EXTI0_INT_PREEMPT_PRI (3)
#define EXTI0_INT_SUB_PRI (0)
#define EXTI1_INT_PREEMPT_PRI (3)
#define EXTI1_INT_SUB_PRI (1)
#define EXTI2_TSC_INT_PREEMPT_PRI (3)
#define EXTI2_TSC_INT_SUB_PRI (2)
#define EXTI3_INT_PREEMPT_PRI (3)
#define EXTI3_INT_SUB_PRI (4)

// Public function definitions
void SystemClock_Config(void);
void configure_pins(void);

#ifdef WATCHDOG_ENABLE
ret_t wdg_init(void);
ret_t wdg_pet(void);
bool wdg_isSet(void);
#endif

void hw_USB_init(void);
void hw_setUSBpullup(uint8_t value);

void button_periodic_handler(uint32_t current_tick);
uint8_t mainbutton_getval(void);

void LED_toggle(uint16_t LED);
void LED_set(uint16_t LED, uint8_t value);

void TimingPin_toggle(void);
void TimingPin_set(uint8_t value);
