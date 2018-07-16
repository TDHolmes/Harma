/*!
 * @file    LSM9DS1.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    24-May-2018
 * @brief   Functions to interface with the accelerometer / magnetometer / gyroscope sensor LSM9DS1.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#include "modules/utilities/queue.c"
#include "modules/utilities/scheduler.h"
#include "peripherals/hardware/hardware.h"
#include "peripherals/I2C/I2C.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"

#include "LSM9DS1.h"

#define ACCEL_GYRO_ADDRESS (0b11010110)  // 0xD6 (no R/W bit)
#define MAG_ADDRESS        (0b00111100)  // 0x1E (no R/W bit)


/* Accelerometer and gyroscope registers */

#define ACT_THS                (0x04)
#define ACT_DUR                (0x05)
#define INT_GEN_CFG_XL         (0x06)
#define INT_GEN_THS_X_XL       (0x07)
#define INT_GEN_THS_Y_XL       (0x08)
#define INT_GEN_THS_Z_XL       (0x09)
#define INT_GEN_DUR_XL         (0x0A)
#define REFERENCE_G            (0x0B)
#define INT1_CTRL              (0x0C)
    #define INT1_IG_G              (1 << 0)
    #define INT1_IG_XL             (1 << 1)
    #define INT1_FSS5              (1 << 2)
    #define INT1_OVR               (1 << 3)
    #define INT1_FTH               (1 << 4)
    #define INT1_Boot              (1 << 5)
    #define INT1_DRDY_G            (1 << 6)
    #define INT1_DRDY_XL           (1 << 7)

#define INT2_CTRL              (0x0D)
    #define INT2_INACT             (1 << 0)
    #define INT2_FSS5              (1 << 2)
    #define INT2_OVR               (1 << 3)
    #define INT2_FTH               (1 << 4)
    #define INT2_DRDY_TEMP         (1 << 5)
    #define INT2_DRDY_G            (1 << 6)
    #define INT2_DRDY_XL           (1 << 7)

#define WHO_AM_I               (0x0F)
    #define WHO_AM_I_EXPECTED      (0b01101000)

#define CTRL_REG1_G            (0x10)
#define CTRL_REG2_G            (0x11)
#define CTRL_REG3_G            (0x12)
#define ORIENT_CFG_G           (0x13)
#define INT_GEN_SRC_G          (0x14)
#define OUT_TEMP_L             (0x15)
#define OUT_TEMP_H             (0x16)
// #define STATUS_REG             (0x17)  // TODO: figure out which reg is actually the status reg
#define OUT_X_LOW_G            (0x18)
#define OUT_X_HIGH_G           (0x19)
#define OUT_Y_LOW_G            (0x1A)
#define OUT_Y_HIGH_G           (0x1B)
#define OUT_Z_LOW_G            (0x1C)
#define OUT_Z_HIGH_G           (0x1D)
#define CTRL_REG4              (0x1E)
#define CTRL_REG5_XL           (0x1F)
#define CTRL_REG6_XL           (0x20)
#define CTRL_REG7_XL           (0x21)
#define CTRL_REG8              (0x22)
#define CTRL_REG9              (0x23)
#define CTRL_REG10             (0x24)
#define INT_GEN_SRC_XL         (0x26)
// #define STATUS_REG             (0x27)
#define OUT_X_LOW_XL           (0x28)
#define OUT_X_HIGH_XL          (0x29)
#define OUT_Y_LOW_XL           (0x2A)
#define OUT_Y_HIGH_XL          (0x2B)
#define OUT_Z_LOW_XL           (0x2C)
#define OUT_Z_HIGH_XL          (0x2D)
#define FIFO_CTRL              (0x2E)
#define FIFO_SRC               (0x2F)
#define INT_GEN_CFG_G          (0x30)
#define INT_GEN_THS_X_LOW_G    (0x31)
#define INT_GEN_THS_X_HIGH_G   (0x32)
#define INT_GEN_THS_Y_LOW_G    (0x33)
#define INT_GEN_THS_Y_HIGH_G   (0x34)
#define INT_GEN_THS_Z_LOW_G    (0x35)
#define INT_GEN_THS_Z_HIGH_G   (0x36)
#define INT_GEN_DUR_G          (0x37)

/* Magnetometer registers */

#define OFFSET_X_REG_L_M  (0x05)
#define OFFSET_X_REG_H_M  (0x06)
#define OFFSET_Y_REG_L_M  (0x07)
#define OFFSET_Y_REG_H_M  (0x08)
#define OFFSET_Z_REG_L_M  (0x09)
#define OFFSET_Z_REG_H_M  (0x0A)
#define WHO_AM_I_M        (0x0F)
    #define WHO_AM_I_M_EXPECTED  (0b00111101)
#define CTRL_REG1_M       (0x20)
#define CTRL_REG2_M       (0x21)
#define CTRL_REG3_M       (0x22)
#define CTRL_REG4_M       (0x23)
#define CTRL_REG5_M       (0x24)
#define STATUS_REG_M      (0x27)
#define OUT_X_L_M         (0x28)
#define OUT_X_H_M         (0x29)
#define OUT_Y_L_M         (0x2A)
#define OUT_Y_H_M         (0x2B)
#define OUT_Z_L_M         (0x2C)
#define OUT_Z_H_M         (0x2D)
#define INT_CFG_M         (0x30)
#define INT_SRC_M         (0x31)
#define INT_THS_L         (0x32)
#define INT_THS_H         (0x33)


// --- important data / globals
extern schedule_t main_schedule;  // TODO: don't like externs. Better way to do this?
LSM9DS1_critical_errors_t CriticalErrors = {
    .INT1_IRQs_missed = 0,
    .INT2_IRQs_missed = 0,
    .DRDY_IRQs_missed = 0,
};

// --- Private functions
ret_t accelDataReadyHandler(int32_t *next_callback_ms);
ret_t gyroDataReadyHandler(int32_t *next_callback_ms);
ret_t magDataReadyHandler(int32_t *next_callback_ms);


ret_t LSM9DS1_init(gyro_ODR_t gyro_ODR, accel_ODR_t accel_ODR)
{
    ret_t ret = RET_OK;
    uint8_t tmp = 0;

    // --- Make sure we can communicate with the chip
    // Check accel/gyro first
    ret = I2C_readData(ACCEL_GYRO_ADDRESS, WHO_AM_I, &tmp, 1);
    if (ret != RET_OK) { return ret; }
    if (tmp != WHO_AM_I_EXPECTED) {
        return RET_COM_ERR;
    }

    // Check mag too
    ret = I2C_readData(MAG_ADDRESS, WHO_AM_I_M, &tmp, 1);
    if (ret != RET_OK) { return ret; }
    if (tmp != WHO_AM_I_M_EXPECTED) {
        return RET_COM_ERR;
    }

    // --- Configure the device properly now

    // Setup interrupts
    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, INT1_CTRL, INT1_DRDY_G, true);
    if (ret != RET_OK) { return ret; }
    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, INT2_CTRL, INT2_DRDY_XL, true);
    if (ret != RET_OK) { return ret; }

    // Setup ODRs, filtering, etc.

    return ret;
}


/*! Function to be ran after DRDY from accel is detected. Should be ran in NON-interrupt context
 */
ret_t accelDataReadyHandler(int32_t *next_callback_ms)
{
    ret_t ret = RET_OK;
    int16_t data[3];
    // Read out the new data
    ret = I2C_readData(ACCEL_GYRO_ADDRESS, OUT_X_LOW_XL, (uint8_t *)data, 3);
    if (ret != RET_OK) { return ret; }

    // Queue it up

    // Check status register

    *next_callback_ms = 0;  // We don't need to be called again until next ISR event
    return ret;
}


/*! Function to be ran after DRDY from gyro is detected. Should be ran in NON-interrupt context
 */
ret_t gyroDataReadyHandler(int32_t *next_callback_ms)
{
    ret_t ret = RET_OK;
    int16_t data[3];
    // Read out the new data
    ret = I2C_readData(ACCEL_GYRO_ADDRESS, OUT_X_LOW_G, (uint8_t *)data, 3);
    if (ret != RET_OK) { return ret; }

    // Queue it up

    // Check status register

    *next_callback_ms = 0;  // We don't need to be called again until next ISR event
    return ret;
}


/*! Function to be ran after DRDY from mag is detected.
 *
 * Note: Should be ran in NON-interrupt context
 */
ret_t magDataReadyHandler(int32_t *next_callback_ms)
{
    ret_t ret = RET_OK;
    int16_t data[3];
    // Read out the new data
    ret = I2C_readData(MAG_ADDRESS, OUT_X_L_M, (uint8_t *)data, 3);
    if (ret != RET_OK) { return ret; }

    // Queue it up

    // Check status register

    *next_callback_ms = 0;  // We don't need to be called again until next ISR event
    return ret;
}


// --- ISR handlers that are called by EXTI IRQ handler in hardware.c

void LSM9DS1_INT1_ISR(void)
{
    // queue up gyro DRDY handler for main to run
    ret_t ret = RET_OK;
    uint8_t sched_id;  // we don't need to know the schedule ID...
    ret = scheduler_add(&main_schedule, 0, gyroDataReadyHandler, &sched_id);
    if (ret != RET_OK) {
        // Critical error! we will miss INT1 ISR
        CriticalErrors.INT1_IRQs_missed += 1;
    }

    // TODO: do a quick read of status here if timing critical? or leave in the DRDY non-ISR context handlers?
}


void LSM9DS1_INT2_ISR(void)
{
    // queue up accel DRDY handler for main to run
    ret_t ret = RET_OK;
    uint8_t sched_id;  // we don't need to know the schedule ID...
    ret = scheduler_add(&main_schedule, 0, accelDataReadyHandler, &sched_id);
    if (ret != RET_OK) {
        // Critical error! we will miss INT1 ISR
        CriticalErrors.INT2_IRQs_missed += 1;
    }
}


void LSM9DS1_DRDY_ISR(void)
{
    // queue up Mag DRDY handler for main to run
    ret_t ret = RET_OK;
    uint8_t sched_id;  // we don't need to know the schedule ID...
    ret = scheduler_add(&main_schedule, 0, magDataReadyHandler, &sched_id);
    if (ret != RET_OK) {
        // Critical error! we will miss INT1 ISR
        CriticalErrors.DRDY_IRQs_missed += 1;
    }
}
