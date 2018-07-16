
#include "common.h"

#pragma once

// --- Public datatypes

// CTRL_REG6_XL
typedef enum {
    k2g_fullscale,
    k16g_fullscale,
    k4g_fullscale,
    k8g_fullscale,
} accel_fullscale_t;

// CTRL_REG2_M
typedef enum {
    k4gauss_fullscale,
    k8gauss_fullscale,
    k12gauss_fullscale,
    k16gauss_fullscale
} mag_fullscale_t;

// CTRL_REG1_G
typedef enum {
    k245DPS_fullscale = 0,
    k500DPS_fullscale = 1,
    k2000DPS_fullscale = 3,
} gyro_fullscale_t;


// CTRL_REG1_G
typedef enum {
    kGyroODR_OFF,
    kGyroODR_14_9_Hz,
    kGyroODR_59_5_Hz,
    kGyroODR_119_Hz,
    kGyroODR_238_Hz,
    kGyroODR_476_Hz,
    kGyroODR_952_Hz
} gyro_ODR_t;


typedef enum {
    kAccelODR_OFF,
    kAccelODR_10_Hz,
    kAccelODR_50_Hz,
    kAccelODR_119_Hz,
    kAccelODR_238_Hz,
    kAccelODR_476_Hz,
    kAccelODR_952_Hz
} accel_ODR_t;


typedef enum {
    kAccel_bandqidth_408_Hz,
    kAccel_bandqidth_211_Hz,
    kAccel_bandqidth_105_Hz,
    kAccel_bandqidth_50_Hz,
    kAccel_bandqidth_automatic,
} accel_bandwidth_t;


typedef struct __attribute__((packed)) {
    uint32_t INT1_IRQs_missed;
    uint32_t INT2_IRQs_missed;
    uint32_t DRDY_IRQs_missed;
} LSM9DS1_critical_errors_t;


// --- Public functions

ret_t LSM9DS1_init(gyro_ODR_t gyro_ODR, accel_ODR_t accel_ODR);
void LSM9DS1_INT1_ISR(void);
void LSM9DS1_INT2_ISR(void);
void LSM9DS1_DRDY_ISR(void);
