/*!
 *
 */
#pragma once

#include "modules/orientation/datatypes.h"
#include "common.h"


#define ACCEL_GYRO_ADDRESS (0b11010110)  // 0xD6 (no R/W bit)
#define MAG_ADDRESS        (0b00111100)  // 0x1E (no R/W bit)


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

ret_t LSM9DS1_init(
    gyro_ODR_t gyro_ODR, gyro_fullscale_t gyro_FS,
    accel_ODR_t accel_ODR, accel_fullscale_t accel_FS);
ret_t LSM9DS1_readStatus(uint8_t * status_byte_ptr);

// Configuration methods...
ret_t LSM9DS1_setAccel_ODR_FS(accel_ODR_t accel_ODR, accel_fullscale_t accel_FS);
ret_t LSM9DS1_setGyro_ODR_FS(gyro_ODR_t gyro_ODR, gyro_fullscale_t gyro_FS);

// ISR handlers

void LSM9DS1_AGINT1_ISR(void);
void LSM9DS1_AGINT2_ISR(void);
void LSM9DS1_MDRDY_ISR(void);

// Accessors of new data
ret_t LSM9DS1_getAccelPacket(accel_norm_t * pkt_destination_ptr);
ret_t LSM9DS1_getGyroPacket(gyro_norm_t * pkt_destination_ptr);
ret_t LSM9DS1_getMagPacket(mag_norm_t * pkt_destination_ptr);
