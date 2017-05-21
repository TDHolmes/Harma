/*!
 * @file    ADC.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Wraper functions around the stm32f3xx HAL driver for the ADC.
 *
 */

#ifndef _ADC_H_
#define _ADC_H_


#include <stdint.h>
#include "common.h"


ret_t ADC_init(void);
ret_t ADC_start_sampling(void);
uint16_t ADC_get_value(void);

#endif /* _ADC_H_ */
