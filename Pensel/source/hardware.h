#ifndef _HARDWARE_H_
#define _HARDWARE_H_


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
#define BUTTON_MAIN        (GPIO_PIN_5)
#define BUTTON_AUX         (GPIO_PIN_4)
#define SWITCH_0           (GPIO_PIN_1)
#define SWITCH_1           (GPIO_PIN_2)
#define SWITCH_2           (GPIO_PIN_3)

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


// Public function definitions
void SystemClock_Config(void);
void configure_pins(void);

void LED_toggle(uint16_t LED);
void LED_set(uint16_t LED, uint8_t value);


#endif /* _HARDWARE_H_ */
