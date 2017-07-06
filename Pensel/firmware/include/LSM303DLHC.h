/*!
 * @file    LSM303DLHC.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Functions and definitions to interface with the accelerometer + magnetometer chip LSM303DLHC.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "common.h"


//! The possible sensitivities of the accel module in the LSM303DLHC.
typedef enum {
    kOne_mg_per_LSB,
    kTwo_mg_per_LSB,
    kFour_mg_per_LSB,
    kTwelve_mg_per_LSB
} accel_sensitivity_t;

//! The possible sensitivities of the mag module in the LSM303DLHC.
typedef enum {
    kXY_1100_Z_980_LSB_per_g = 1,
    kXY_855_Z_760_LSB_per_g,
    kXY_670_Z_600_LSB_per_g,
    kXY_450_Z_400_LSB_per_g,
    kXY_400_Z_355_LSB_per_g,
    kXY_330_Z_295_LSB_per_g,
    kXY_230_Z_205_LSB_per_g
} mag_sensitivity_t;

//! The possible output data rates of the accel module in the LSM303DLHC.
typedef enum {
    kAccelODR_PowerDown,
    kAccelODR_1Hz,
    kAccelODR_10_Hz,
    kAccelODR_25_Hz,
    kAccelODR_50_Hz,
    kAccelODR_100_Hz,
    kAccelODR_200_Hz,
    kAccelODR_400_Hz,
    kAccelODR_1620_Hz,
    kAccelODR_N_1344_LPM_5376_Hz
} accel_ODR_t;

//! The possible output data rates of the mag module in the LSM303DLHC.
typedef enum {
    kMagODR_0_75_Hz,
    kMagODR_1_5_Hz,
    kMagODR_3_Hz,
    kMagODR_7_5_Hz,
    kMagODR_15_Hz,
    kMagODR_30_Hz,
    kMagODR_75_Hz,
    kMagODR_220_Hz
} mag_ODR_t;

//! The possible modes we can configure the FIFO in the LSM303DLHC.
typedef enum {
    kFIFO_MODE_Bypass,
    kFIFO_MODE_FIFO,
    kFIFO_MODE_Stream,
    kFIFO_MODE_Trigger
} FIFO_mode_t;

//! Raw accel packet definition
typedef struct __attribute__((packed)) {
    uint32_t frame_num;   //!< Frame number
    uint32_t timestamp;   //!< Timestamp when the packet was received
    int16_t x;            //!< Accel X value
    int16_t y;            //!< Accel Y value
    int16_t z;            //!< Accel Z value
} accel_raw_t;

//! Normalized accel packet definition
typedef struct __attribute__((packed)) {
    uint32_t frame_num;   //!< Frame number
    uint32_t timestamp;  //!< Timestamp when the packet was received
    float x;             //!< Accel X value
    float y;             //!< Accel Y value
    float z;             //!< Accel Z value
} accel_norm_t;

//! Raw mag packet definition
typedef struct __attribute__((packed)) {
    uint32_t frame_num;   //!< Frame number
    uint32_t timestamp;   //!< Timestamp when the packet was received
    int16_t x;            //!< Mag X value
    int16_t y;            //!< Mag Y value
    int16_t z;            //!< Mag Z value
} mag_raw_t;

//! Normalized mag packet definition
typedef struct __attribute__((packed)) {
    uint32_t frame_num;   //!< Frame number
    uint32_t timestamp;  //!< Timestamp when the packet was received
    float x;             //!< Mag X value
    float y;             //!< Mag Y value
    float z;             //!< Mag Z value
} mag_norm_t;


ret_t LSM303DLHC_init(accel_ODR_t accel_datarate, accel_sensitivity_t accel_sensitivity,
                      mag_ODR_t mag_datarate, mag_sensitivity_t mag_sensitivity);

// Function to take care of getting packets after an ISR is handled
ret_t LSM303DLHC_run(void);

// functions to get packets and check for data!
bool LSM303DLHC_accel_dataAvailable(void);
bool LSM303DLHC_mag_dataAvailable(void);
ret_t LSM303DLHC_accel_getPacket(accel_norm_t * pkt_ptr, bool peak);
ret_t LSM303DLHC_mag_getPacket(mag_norm_t * pkt_ptr, bool peak);
// error counting methods...
uint32_t LSM303DLHC_accel_packetOverwriteCount(void);
uint32_t LSM303DLHC_mag_packetOverwriteCount(void);
uint32_t LSM303DLHC_accel_HardwareOverwriteCount(void);
uint32_t LSM303DLHC_mag_HardwareOverwriteCount(void);

// direct call to get the temperature
ret_t LSM303DLHC_temp_getData(int16_t * temp_val_ptr);

// functions to be called by the hardware pin interrupt handler
void LSM303DLHC_drdy_ISR(void);
// void LSM303DLHC_int_handler(void);

ret_t rpt_LSM303DLHC_enableStreams(uint8_t * in_p, uint8_t in_len,
                                   uint8_t * UNUSED_PARAM(out_p),
                                   uint8_t * UNUSED_PARAM(out_len_ptr));
ret_t rpt_LSM303DLHC_changeConfig(uint8_t * in_p, uint8_t in_len,
                                  uint8_t * UNUSED_PARAM(out_p),
                                  uint8_t * UNUSED_PARAM(out_len_ptr));
ret_t rpt_LSM303DLHC_getTemp(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                             uint8_t * out_p, uint8_t * out_len_ptr);
ret_t rpt_LSM303DLHC_getAccel(uint8_t * in_p, uint8_t in_len,
                              uint8_t * out_p, uint8_t * out_len_ptr);
ret_t rpt_LSM303DLHC_getMag(uint8_t * in_p, uint8_t in_len,
                            uint8_t * out_p, uint8_t * out_len_ptr);
ret_t rpt_LSM303DLHC_getErrors(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                               uint8_t * out_p, uint8_t * out_len_ptr);
