/*!
 * @file    ADC.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Wraper functions around the stm32f3xx HAL driver for the ADC.
 *
 */

 // User includes
 #include <stdint.h>
 #include <stdbool.h>
 #include "common.h"

 // STM drivers
 #include "peripherals/stm32f3-configuration/stm32f3xx.h"
 #include "peripherals/stm32f3/stm32f3xx_hal_def.h"
 #include "peripherals/stm32f3/stm32f3xx_hal.h"
 #include "peripherals/stm32f3/stm32f3xx_hal_adc.h"
 #include "peripherals/stm32f3/stm32f3xx_hal_adc_ex.h"

 #include "ADC.h"


/* Definitions of data related to this example */
/* Definition of ADCx conversions data table size */
#define ADC_NUM_SAMPLES   ((uint32_t)  4)   /* Size of array ADC_converted_data[] */

/* ADC handle declaration */
ADC_HandleTypeDef       AdcHandle;

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef  sConfig;

/* Variable containing ADC conversions data */
static uint16_t   ADC_converted_data[ADC_NUM_SAMPLES];

// boolean and index to keep track if we're sampling
uint8_t gNum_samples;
bool    gADC_sampling;


ret_t ADC_init(void)
{
    gNum_samples = 0;
    gADC_sampling = false;

    // First, dinitialize it
    AdcHandle.Instance          = ADC1;
    if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
    {
        return RET_GEN_ERR;
    }

    AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;      /* Synchronous clock mode, input ADC clock divided by 2*/
    AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
    AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
    AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
    AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
    AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled (no automatic conversion restart after each conversion) */
    AdcHandle.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
    AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
    AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
    AdcHandle.Init.DMAContinuousRequests = ENABLE;                        /* ADC DMA continuous request to match with DMA circular mode */
    AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
    /* Initialize ADC peripheral according to the passed parameters */
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
        return RET_GEN_ERR;
    }

    // Calibrate the pin
    if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) !=  HAL_OK) {
        return RET_GEN_ERR;
    }

    // configure which channel to use
    sConfig.Channel      = ADC_CHANNEL_1;                /* Sampled channel number */
    sConfig.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADC_CHANNEL */
    sConfig.SamplingTime = ADC_SAMPLETIME_61CYCLES_5;   /* Sampling time (number of clock cycles unit) */
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
    sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */
    sConfig.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */

    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        return RET_GEN_ERR;
    }

    return RET_OK;
}

ret_t ADC_start_sampling(void)
{
    if (HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)ADC_converted_data,
                          ADC_NUM_SAMPLES) != HAL_OK) {
        return RET_GEN_ERR;
    } else {
        gADC_sampling = true;
        return RET_OK;
    }
}


uint16_t ADC_get_value(void)
{
    uint8_t i;
    uint16_t return_value = 0;

    // check if we're still sampling
    if (gADC_sampling) {
        return 0;
    } else {
        for (i = 0; i <ADC_NUM_SAMPLES; i++) {
            return_value += ADC_converted_data[i];
        }

        // Hopefully this divide isn't too costly... should simplify to a bitshift
        return (return_value / ADC_NUM_SAMPLES);
    }
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    gNum_samples += 1;
    if (gNum_samples == ADC_NUM_SAMPLES) {
        gNum_samples = 0;
        gADC_sampling = false;
    }
}
