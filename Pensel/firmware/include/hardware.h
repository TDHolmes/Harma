/*!
 * @file    hardware.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Hardware specific definitions and function calls.
 *
 */

#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "stm32f3xx.h"

// Sensor stuff
#define SENSOR_PORT (GPIOA)
#define SENSOR_DRDY (GPIO_PIN_11)
#define SENSOR_INT  (GPIO_PIN_12)

// LED / exposed pad stuff
#define LED_PORT   (GPIOA)
#define LED_0      (GPIO_PIN_6)
#define LED_1      (GPIO_PIN_7)
#define EXTRA_GPIO (GPIO_PIN_8)

// Button & switch stuff
#define SWITCH_BUTTON_PORT (GPIOA)
#define SWITCH_0           (GPIO_PIN_1)
#define SWITCH_1           (GPIO_PIN_2)
#define SWITCH_2           (GPIO_PIN_3)
#define BUTTON_AUX         (GPIO_PIN_4)
#define BUTTON_MAIN        (GPIO_PIN_5)

// Thumbwheel
#define THUMBWHEEL_PORT (GPIOA)
#define THUMBWHEEL      (GPIO_PIN_0)

// I2C
#define I2C_PORT    (GPIOF)
#define I2C_SDA     (GPIO_PIN_0)
#define I2C_SCL     (GPIO_PIN_1)

// UART
#define UART_PORT   (GPIOA)
#define UART_RX     (GPIO_PIN_10)
#define UART_TX     (GPIO_PIN_9)


//! Keeps track of the state of the 3 position switch.
typedef enum {
    kSwitch_0,
    kSwitch_1,
    kSwitch_2
} switch_state_t;


// Public function definitions
void SystemClock_Config(void);
void configure_pins(void);


ret_t wdg_init(void);
ret_t wdg_pet(void);
bool wdg_isSet(void);

void button_periodic_handler(uint32_t current_tick);
void switch_periodic_handler(uint32_t current_tick);
switch_state_t switch_getval(void);
uint8_t mainbutton_getval(void);
uint8_t auxbutton_getval(void);

void LED_toggle(uint16_t LED);
void LED_set(uint16_t LED, uint8_t value);

void TimingPin_toggle(void);
void TimingPin_set(uint8_t value);


#endif /* _HARDWARE_H_ */
