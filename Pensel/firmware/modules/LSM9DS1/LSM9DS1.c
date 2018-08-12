/*!
 * @file    LSM9DS1.c
 * @author  Tyler Holmes
 *
 * @date    24-May-2018
 * @brief   Functions to interface with the accelerometer / magnetometer / gyroscope sensor LSM9DS1.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#include "modules/utilities/newqueue.h"
#include "modules/utilities/scheduler.h"
#include "modules/utilities/logging.h"

#include "peripherals/hardware/hardware.h"
#include "peripherals/I2C/I2C.h"
#include "peripherals/stm32f3/stm32f3xx_hal.h"

#include "peripherals/UART/UART.h"

#include "LSM9DS1.h"

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
    #define INT1_IG_G              (1 << 7)
    #define INT1_IG_XL             (1 << 6)
    #define INT1_FSS5              (1 << 5)
    #define INT1_OVR               (1 << 4)
    #define INT1_FTH               (1 << 3)
    #define INT1_Boot              (1 << 2)
    #define INT1_DRDY_G            (1 << 1)
    #define INT1_DRDY_XL           (1 << 0)

#define INT2_CTRL              (0x0D)
    #define INT2_INACT             (1 << 7)
    #define INT2_FSS5              (1 << 5)
    #define INT2_OVR               (1 << 4)
    #define INT2_FTH               (1 << 3)
    #define INT2_DRDY_TEMP         (1 << 2)
    #define INT2_DRDY_G            (1 << 1)
    #define INT2_DRDY_XL           (1 << 0)

#define WHO_AM_I               (0x0F)
    #define WHO_AM_I_EXPECTED      (0b01101000)

#define CTRL_REG1_G            (0x10)
    #define GYRO_ODR_SHIFT         (5)
    #define GYRO_FS_SHIFT          (3)

#define CTRL_REG2_G            (0x11)
#define CTRL_REG3_G            (0x12)
#define ORIENT_CFG_G           (0x13)
#define INT_GEN_SRC_G          (0x14)
#define OUT_TEMP_L             (0x15)
#define OUT_TEMP_H             (0x16)
#define STATUS_REG_G           (0x17)
#define OUT_X_LOW_G            (0x18)
#define OUT_X_HIGH_G           (0x19)
#define OUT_Y_LOW_G            (0x1A)
#define OUT_Y_HIGH_G           (0x1B)
#define OUT_Z_LOW_G            (0x1C)
#define OUT_Z_HIGH_G           (0x1D)
#define CTRL_REG4              (0x1E)
#define CTRL_REG5_XL           (0x1F)
#define CTRL_REG6_XL           (0x20)
    #define ACCEL_ODR_SHIFT        (5)
    #define ACCEL_FS_SHIFT         (3)

#define CTRL_REG7_XL           (0x21)
#define CTRL_REG8              (0x22)
#define CTRL_REG9              (0x23)
#define CTRL_REG10             (0x24)
#define INT_GEN_SRC_XL         (0x26)
#define STATUS_REG_XL          (0x27)
    #define STATUS_BOOT_NOT_FINISHED_MASK  (1 << 4)
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

// Other important definitions
#define NUM_ACCEL_PACKETS   (10)  //!< Number of accel packets we'll hold in our queue
#define NUM_MAG_PACKETS     (10)  //!< Number of mag packets we'll hold in our queue
#define NUM_GYRO_PACKETS    (10)  //!< Number of gyro packets we'll hold in our queue


// --- important data / globals
extern schedule_t gMainSchedule;  // TODO: don't like externs. Better way to do this?

typedef struct {
    newqueue_t mag_queue;
    newqueue_t gyro_queue;
    newqueue_t accel_queue;
    uint32_t mag_framenum;
    uint32_t gyro_framenum;
    uint32_t accel_framenum;
    gyro_ODR_t gyro_ODR;
    gyro_fullscale_t gyro_FS;
    accel_ODR_t accel_ODR;
    accel_fullscale_t accel_FS;
    // mag_ODR_t mag_ODR;
    // mag_fullscale_t mag_FS;
    LSM9DS1_critical_errors_t errors;
} LSM9DS1_admin_t;

static LSM9DS1_admin_t gLSM9DS1Admin;

// --- Private functions
ret_t accelDataReadyHandler(int32_t *next_callback_ms);
ret_t gyroDataReadyHandler(int32_t *next_callback_ms);
ret_t magDataReadyHandler(int32_t *next_callback_ms);

static void normalizeAccel(accel_raw_t * raw_pkt, accel_norm_t * norm_pkt_ptr);
static void normalizeMag(mag_raw_t * raw_pkt, mag_norm_t * norm_pkt_ptr);

void enableSensorInterrupts(void);
void disableSensorInterrupts(void);


#define CHECK_RET(__RETVAL__)  ( __RETVAL__ != RET_OK ? fatal_error_handler(__FILE__, __LINE__, __RETVAL__) : 0 )


ret_t LSM9DS1_init(gyro_ODR_t gyro_ODR, gyro_fullscale_t gyro_FS,
                   accel_ODR_t accel_ODR, accel_fullscale_t accel_FS)
                   // mag_ODR_t mag_ODR, mag_fullscale_t mag_FS)
{
    ret_t ret = RET_OK;
    uint8_t tmp = 0;

    // Initialize the admin struct
    newqueue_init(&gLSM9DS1Admin.mag_queue, NUM_MAG_PACKETS, sizeof(mag_norm_t));
    newqueue_init(&gLSM9DS1Admin.gyro_queue, NUM_GYRO_PACKETS, sizeof(gyro_norm_t));
    newqueue_init(&gLSM9DS1Admin.accel_queue, NUM_ACCEL_PACKETS, sizeof(accel_norm_t));

    gLSM9DS1Admin.errors.INT1_IRQs_missed = 0;
    gLSM9DS1Admin.errors.INT2_IRQs_missed = 0;
    gLSM9DS1Admin.errors.DRDY_IRQs_missed = 0;
    gLSM9DS1Admin.mag_framenum = 0;
    gLSM9DS1Admin.gyro_framenum = 0;
    gLSM9DS1Admin.accel_framenum = 0;

    disableSensorInterrupts();

    // --- Make sure we can communicate with the chip
    // Check accel/gyro first
    ret = I2C_readData(ACCEL_GYRO_ADDRESS, WHO_AM_I, &tmp, 1);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }
    if (tmp != WHO_AM_I_EXPECTED) {
        return RET_COM_ERR;
    }

    // Check mag too
    ret = I2C_readData(MAG_ADDRESS, WHO_AM_I_M, &tmp, 1);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }
    if (tmp != WHO_AM_I_M_EXPECTED) {
        return RET_COM_ERR;
    }

    // Reboot
    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, CTRL_REG8, 0x01, true);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    while (1) {
        ret = I2C_readData(ACCEL_GYRO_ADDRESS, STATUS_REG_XL, &tmp, 1);
        CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }
        if ((tmp & !STATUS_BOOT_NOT_FINISHED_MASK) == 0) {
            // We've booted!
            break;
        }
    }

    // --- Configure the device properly now

    // TODO: look into HPF enable - CTRL_REG2_G / CTRL_REG3_G
    // TODO: add bandwidth selection for anti-alias?
    ret = LSM9DS1_setAccel_ODR_FS(accel_ODR, accel_FS);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    // Configure gyro ODR / FS
    // TODO: bandwidth selection?
    ret = LSM9DS1_setGyro_ODR_FS(gyro_ODR, gyro_FS);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    // Enable accel output
    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, CTRL_REG5_XL, 0b00111000, true);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    // TODO: setup mag
    // ret = I2C_writeByte(MAG_ADDRESS, CTRL_REG3_M, 0b00000011, true);
    enableSensorInterrupts();

    // Setup interrupts
    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, INT1_CTRL, INT1_DRDY_G, true);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }
    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, INT2_CTRL, INT2_DRDY_XL, true);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    int32_t i;
    accelDataReadyHandler(&i);
    gyroDataReadyHandler(&i);

    return ret;
}


void enableSensorInterrupts(void)
{
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI0_IRQn));
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI1_IRQn));
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI2_TSC_IRQn));
}


void disableSensorInterrupts(void)
{
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI0_IRQn));
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI1_IRQn));
    HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI2_TSC_IRQn));
}


ret_t LSM9DS1_setAccel_ODR_FS(accel_ODR_t accel_ODR, accel_fullscale_t accel_FS)
{
    ret_t ret;
    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, CTRL_REG6_XL,
        (accel_ODR << ACCEL_ODR_SHIFT) | (accel_FS << ACCEL_FS_SHIFT), true);
    if (ret == RET_OK) {
        gLSM9DS1Admin.accel_ODR = accel_ODR;
        gLSM9DS1Admin.accel_FS = accel_FS;
    }
    return ret;
}


ret_t LSM9DS1_setGyro_ODR_FS(gyro_ODR_t gyro_ODR, gyro_fullscale_t gyro_FS)
{
    ret_t ret;

    ret = I2C_writeByte(ACCEL_GYRO_ADDRESS, CTRL_REG1_G,
        (gyro_ODR << GYRO_ODR_SHIFT) | (gyro_FS << GYRO_FS_SHIFT), true);
    if (ret == RET_OK) {
        gLSM9DS1Admin.gyro_ODR = gyro_ODR;
        gLSM9DS1Admin.gyro_FS = gyro_FS;
    }

    return ret;
}



ret_t LSM9DS1_readStatus(uint8_t * status_byte_ptr)
{
    return I2C_readData(ACCEL_GYRO_ADDRESS, STATUS_REG_XL, status_byte_ptr, 1);
}


/*! Function to be ran after DRDY from accel is detected. Should be ran in NON-interrupt context
 */
ret_t accelDataReadyHandler(int32_t *next_callback_ms)
{
    ret_t ret = RET_OK;
    int16_t data[3];
    accel_raw_t rawPkt;
    accel_norm_t normPkt;

    LOG_MSG(kLogLevelDebug, "DRH - Accel");

    // We don't need to be called again until next ISR event
    *next_callback_ms = SCHEDULER_FINISHED;

    // Read out the new data
    ret = I2C_readData(ACCEL_GYRO_ADDRESS, OUT_X_LOW_XL, (uint8_t *)data, 6);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    // Build up a packet
    rawPkt.header.timestamp = HAL_GetTick();
    rawPkt.header.frame_num = gLSM9DS1Admin.accel_framenum;
    gLSM9DS1Admin.accel_framenum += 1;
    rawPkt.x = data[0];
    rawPkt.y = data[1];
    rawPkt.z = data[2];

    // Normalize and queue it up
    normalizeAccel(&rawPkt, &normPkt);
    newqueue_push(&gLSM9DS1Admin.accel_queue, &normPkt, 1);

    return ret;
}


/*! Function to be ran after DRDY from gyro is detected. Should be ran in NON-interrupt context
 */
ret_t gyroDataReadyHandler(int32_t *next_callback_ms)
{
    ret_t ret = RET_OK;
    int16_t data[3];

    LOG_MSG(kLogLevelDebug, "DRH - Gyro");

    // We don't need to be called again until next ISR event
    *next_callback_ms = SCHEDULER_FINISHED;

    // Read out the new data
    // UART_sendString("g");
    ret = I2C_readData(ACCEL_GYRO_ADDRESS, OUT_X_LOW_G, (uint8_t *)data, 6);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    // Queue it up

    // Check status register
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
    mag_raw_t rawPkt;
    mag_norm_t normPkt;

    LOG_MSG(kLogLevelDebug, "DRH - Mag");

    // We don't need to be called again until next ISR event
    *next_callback_ms = SCHEDULER_FINISHED;

    // Read out the new data
    ret = I2C_readData(MAG_ADDRESS, OUT_X_L_M, (uint8_t *)data, 6);
    CHECK_RET(ret);  // if (ret != RET_OK) { return ret; }

    // Build up a packet
    rawPkt.header.timestamp = HAL_GetTick();
    rawPkt.header.frame_num = gLSM9DS1Admin.mag_framenum;
    gLSM9DS1Admin.mag_framenum += 1;
    rawPkt.x = data[0];
    rawPkt.y = data[1];
    rawPkt.z = data[2];

    // Normalize and queue it up
    normalizeMag(&rawPkt, &normPkt);
    newqueue_push(&gLSM9DS1Admin.mag_queue, &normPkt, 1);
    return ret;
}


// -- Higher level data manipulation Functions

/*! Function for normalizing raw packets into packets normalized to 1g
 */
static void normalizeAccel(accel_raw_t * raw_pkt, accel_norm_t * norm_pkt_ptr)
{
    norm_pkt_ptr->header.frame_num = raw_pkt->header.frame_num;
    norm_pkt_ptr->header.timestamp = raw_pkt->header.timestamp;
    norm_pkt_ptr->x = (float)raw_pkt->x;
    norm_pkt_ptr->y = (float)raw_pkt->y;
    norm_pkt_ptr->z = (float)raw_pkt->z;

    // TODO: Normalize the gain
    // norm_pkt_ptr->x *= AccelGainOffsets[LSM303DLHC.accel_sensitivity];
    // norm_pkt_ptr->y *= AccelGainOffsets[LSM303DLHC.accel_sensitivity];
    // norm_pkt_ptr->z *= AccelGainOffsets[LSM303DLHC.accel_sensitivity];
}


static void normalizeMag(mag_raw_t * raw_pkt, mag_norm_t * norm_pkt_ptr)
{
    norm_pkt_ptr->header.frame_num = raw_pkt->header.frame_num;
    norm_pkt_ptr->header.timestamp = raw_pkt->header.timestamp;
    norm_pkt_ptr->x = (float)raw_pkt->x;
    norm_pkt_ptr->y = (float)raw_pkt->y;
    norm_pkt_ptr->z = (float)raw_pkt->z;

    // TODO: Normalize the gain
    // norm_pkt_ptr->x *= MagGainOffsets_XY[LSM303DLHC.mag_sensitivity - 1];
    // norm_pkt_ptr->y *= MagGainOffsets_XY[LSM303DLHC.mag_sensitivity - 1];
    // norm_pkt_ptr->z *= MagGainOffsets_Z[LSM303DLHC.mag_sensitivity - 1];
}


// --- ISR handlers that are called by EXTI IRQ handler in hardware.c

void LSM9DS1_AGINT1_ISR(void)
{
    // queue up gyro DRDY handler for main to run
    ret_t ret = RET_OK;
    uint8_t sched_id;  // we don't need to know the schedule ID...
    ret = scheduler_add(&gMainSchedule, 0, gyroDataReadyHandler, &sched_id);
    if (ret != RET_OK) {
        // Critical error! we will miss INT1 ISR
        gLSM9DS1Admin.errors.INT1_IRQs_missed += 1;
    }

    // TODO: do a quick read of status here if timing critical? or leave in the DRDY non-ISR context handlers?
}


void LSM9DS1_AGINT2_ISR(void)
{
    // queue up accel DRDY handler for main to run
    ret_t ret = RET_OK;
    uint8_t sched_id;  // we don't need to know the schedule ID...
    ret = scheduler_add(&gMainSchedule, 0, accelDataReadyHandler, &sched_id);
    if (ret != RET_OK) {
        // Critical error! we will miss INT1 ISR
        gLSM9DS1Admin.errors.INT2_IRQs_missed += 1;
    }
}


void LSM9DS1_MDRDY_ISR(void)
{
    // queue up Mag DRDY handler for main to run
    ret_t ret = RET_OK;
    uint8_t sched_id;  // we don't need to know the schedule ID..
    ret = scheduler_add(&gMainSchedule, 0, magDataReadyHandler, &sched_id);
    if (ret != RET_OK) {
        // Critical error! we will miss INT1 ISR
        gLSM9DS1Admin.errors.DRDY_IRQs_missed += 1;
    }
}


// --- Accessor methods



ret_t LSM9DS1_getAccelPacket(accel_norm_t * pkt_destination_ptr)
{
    return newqueue_pop(&gLSM9DS1Admin.accel_queue, pkt_destination_ptr, 1, eNoPeak);
}


ret_t LSM9DS1_getGyroPacket(gyro_norm_t * pkt_destination_ptr)
{
    return newqueue_pop(&gLSM9DS1Admin.gyro_queue, pkt_destination_ptr, 1, eNoPeak);
}


ret_t LSM9DS1_getMagPacket(mag_norm_t * pkt_destination_ptr)
{
    return newqueue_pop(&gLSM9DS1Admin.mag_queue, pkt_destination_ptr, 1, eNoPeak);
}
