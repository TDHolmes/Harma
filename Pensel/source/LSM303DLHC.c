/*
 *
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "I2C.h"
#include "LSM303DLHC.h"


#define ACCEL_ADDRESS (0b0011001)
#define MAG_ADDRESS   (0b0011110)

#define ACCEL_QUEUE_SIZE (8)
#define MAG_QUEUE_SIZE   (4)

/*
Mag sensor address behavior:
    If (address pointer = 08) then the address pointer = 03
    Or else, if (address pointer >= 12) then the address pointer = 0
    Or else, (address pointer) = (address pointer) + 1
*/


// 7.1 Linear acceleration register description
#define ACCEL_CTRL_REG1_A     (0x20)
#define ACCEL_CTRL_REG2_A     (0x21)
#define ACCEL_CTRL_REG3_A     (0x22)
#define ACCEL_CTRL_REG4_A     (0x23)
#define ACCEL_CTRL_REG5_A     (0x24)
#define ACCEL_CTRL_REG6_A     (0x25)
#define ACCEL_REFERENCE_A     (0x26)
#define ACCEL_STATUS_REG_A    (0x27)
#define ACCEL_OUT_X_L_A       (0x28)
#define ACCEL_OUT_X_H_A       (0x29)
#define ACCEL_OUT_Y_L_A       (0x2A)
#define ACCEL_OUT_Y_H_A       (0x2B)
#define ACCEL_OUT_Z_L_A       (0x2C)
#define ACCEL_OUT_Z_H_A       (0x2D)
#define ACCEL_FIFO_CTRL_REG_A (0x2E)
#define ACCEL_FIFO_SRC_REG_A  (0x2F)
#define ACCEL_INT1_CFG_A      (0x30)
#define ACCEL_INT1_SRC_A      (0x31)
#define ACCEL_INT1_THS_A      (0x32)
#define ACCEL_INT1_DURATION_A (0x33)
#define ACCEL_INT2_CFG_A      (0x34)
#define ACCEL_INT2_SRC_A      (0x35)
#define ACCEL_INT2_THS_A      (0x36)
#define ACCEL_INT2_DURATION_A (0x37)
#define ACCEL_CLICK_CFG_A     (0x38)
#define ACCEL_CLICK_SRC_A     (0x39)
#define ACCEL_CLICK_THS_A     (0x3A)
#define ACCEL_TIME_LIMIT_A    (0x3B)
#define ACCEL_TIME_LATENCY_A  (0x3C)
#define ACCEL_TIME_WINDOW_A   (0x3D)

// 7.2 Magnetic field sensing register description
#define MAG_CRA_REG_M     (0x00)
#define MAG_CRB_REG_M     (0x01)
#define MAG_MR_REG_M      (0x02)
#define MAG_OUT_X_H_M     (0x03)
#define MAG_OUT_X_L_M     (0x04)
#define MAG_OUT_Z_H_M     (0x05)
#define MAG_OUT_Z_L_M     (0x06)
#define MAG_OUT_Y_H_M     (0x07)
#define MAG_OUT_Y_L_M     (0x08)
#define MAG_SR_REG_M      (0x09)
#define MAG_IR1_REG_M     (0x0A)
#define MAG_IR2_REG_M     (0x0B)
#define MAG_IR3_REG_M     (0x0C)
#define MAG_TEMP_OUT_H_M  (0x31)
#define MAG_TEMP_OUT_L_M  (0x32)

/* Register definitions:
    CTRL_REG1_A
        7-4: ODR[3:0]   Data rate selection. Default value: 0000 (0000: power-down, others: refer to Table 20)
        3: LPen         Low-power mode enable. Default value: 0 (0: normal mode, 1: low-power mode)
        2: Zen          Z-axis enable. Default value: 1   (0: Z-axis disabled, 1: Z-axis enabled)
        1: Yen          Y-axis enable. Default value: 1   (0: Y-axis disabled, 1: Y-axis enabled)
        0: Xen          X-axis enable. Default value: 1   (0: X-axis disabled, 1: X-axis enabled)

    CTRL_REG2_A
        7-6: HPM[1:0]   High-pass filter mode selection. Default value: 00 (refer to Table 23)
        5-4: HPCF[2:1]  High-pass filter cutoff frequency selection
        3:   FDS        Filtered data selection. Default value: 0
                             (0: internal filter bypassed, 1: data from internal filter sent to output register and FIFO)
        2:   HPCLICK    High-pass filter enabled for click function (0: filter bypassed, 1: filter enabled)
        1:   HPIS2      High-pass filter enabled for AOI function on Interrupt 2 (0: filter bypassed, 1: filter enabled)
        0:   HPIS1      High-pass filter enabled for AOI function on Interrupt 1 (0: filter bypassed, 1: filter enabled)

    CTRL_REG3_A
        7: I1_CLICK   CLICK interrupt on INT1. Default value 0 (0: disable, 1: enable)
        6: I1_AOI1    AOI1 interrupt on INT1. Default value 0 (0: disable, 1: enable)
        5: I1_AOI2    AOI2 interrupt on INT1. Default value 0 (0: disable, 1: enable)
        4: I1_DRDY1   DRDY1 interrupt on INT1. Default value 0 (0: disable, 1: enable)
        3: I1_DRDY2   DRDY2 interrupt on INT1. Default value 0 (0: disable, 1: enable)
        2: I1_WTM     FIFO watermark interrupt on INT1. Default value 0 (0: disable, 1: enable)
        1: I1_OVERRUN FIFO overrun interrupt on INT1. Default value 0 (0: disable, 1: enable)

    CTRL_REG4_A
        7:   BDU      Block data update. Default value: 0   (0: continuous update,
                             1: output registers not updated until MSB and LSB have been read
        6:   BLE      Big/little endian data selection. Default value 0.   (0: data LSB @ lower address, 1: data MSB @ lower address)
        5-4: FS[1:0]  Full-scale selection. Default value: 00 (00: ±2 g, 01: ±4 g, 10: ±8 g, 11: ±16 g)
        3:   HR       High-resolution output mode: Default value: 0   (0: high-resolution disable, 1: high-resolution enable)
        1-2: ZERO     (MUST ALWAYS BE SET TO ZERO)
        0:   SIM      SPI serial interface mode selection. Default value: 0 (0: 4-wire interface, 1: 3-wire interface).

    CTRL_REG5_A
        7:   BOOT       Reboot memory content. Default value: 0 (0: normal mode, 1: reboot memory content)
        6:   FIFO_EN    FIFO enable. Default value: 0   (0: FIFO disable, 1: FIFO enable)
        5-4: DONTCARE
        3:   LIR_INT1   Latch interrupt request on INT1_SRC register, with INT1_SRC register cleared by reading INT1_SRC itself.
                             Default value: 0.   (0: interrupt request not latched, 1: interrupt request latched)
        2:   D4D_INT1   4D enable: 4D detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1.
        1:   LIR_INT2   Latch interrupt request on INT2_SRC register, with INT2_SRC register cleared by reading INT2_SRC itself.
                             Default value: 0.   (0: interrupt request not latched, 1: interrupt request latched)
        0:   D4D_INT2   4D enable: 4D detection is enabled on INT2 when 6D bit on INT2_CFG is set to 1.

    CTRL_REG6_A
        7:    I2_CLICKen   CLICK interrupt on PAD2. Default value 0. (0: disable, 1: enable)
        6:    I2_INT1      Interrupt 1 on PAD2. Default value 0. (0: disable, 1: enable)
        5:    I2_INT2      Interrupt 2 on PAD2. Default value 0. (0: disable, 1: enable)
        4:    BOOT_I1      Reboot memory content on PAD2. Default value: 0 (0: disable, 1: enable)
        3:    P2_ACT       Active function status on PAD2. Default value 0 (0: disable, 1: enable)
        2, 0: DONTCARE
        1:    H_LACTIVE    Interrupt active high, low. Default value 0. (0: active high, 1: active low)

    REFERENCE_A
        7-0: Ref[7:0]  Reference value for interrupt generation. Default value: 0000 0000

    STATUS_A
        7: ZYXOR   X-, Y-, and Z-axis data overrun.
        6: ZOR     Z-axis data overrun.
        5: YOR     Y-axis data overrun.
        4: XOR     X-axis data overrun.
        3: ZYXDA   X-, Y-, and Z-axis new data available.
        2: ZDA     Z-axis new data available.
        1: YDA     Y-axis new data available.
        0: XDA     X-axis new data available.

    OUT_X_L_A (28h), OUT_X_H_A (29h)
        X-axis acceleration data. The value is expressed in two’s complement.
    OUT_Y_L_A (2Ah), OUT_Y_H_A (2Bh)
        Y-axis acceleration data. The value is expressed in two’s complement.
    OUT_Z_L_A (2Ch), OUT_Z_H_A (2Dh)
        Z-axis acceleration data. The value is expressed in two’s complement.

    FIFO_CTRL_REG_A
        7-6: FM[1:0]   FIFO mode selection. Default value: 00 (see Table 38)
        5:   TR        Trigger selection. Default value: 0   0: trigger event linked to trigger signal on INT1
                             1: trigger event linked to trigger signal on INT2
        4-0: FTH[4:0]  Default value: 00000

    FIFO_SRC_REG_A
        7: WTM - Watermark
        6: OVRN_FIFO - Overrun
        5: EMPTY - FIFO Empty
        4: FSS4 - No Clue
        3: FSS3 - No Clue
        2: FSS2 - No Clue
        1: FSS1 - No Clue
        0: FSS0 - No Clue


    CRA_REG_M
        7:   TEMP _EN   Temperature sensor enable. 0: temperature sensor disabled (default), 1: temperature sensor enabled
        4-2: DO[2:0]    Data output rate bits. These bits set the rate at which data is written to
                             all three data output registers (refer to Table 72). Default value: 100


    CRB_REG_M
        7-5: GN[2:0] Gain configuration bits. The gain configuration is common for all channels
        4-0: Must be zero!

    MR_REG_M
        7-2: Must be zero!
        1-0: MD[1:0] Mode select bits. These bits select the operation mode of this device
                0 0 Continuous-conversion mode
                0 1 Single-conversion mode
                1 0 Sleep mode. Device is placed in sleep mode
                1 1 Sleep mode. Device is placed in sleep mode

    OUT_X_H_M (03), OUT_X_L_M (04h)
        X-axis magnetic field data. The value is expressed as two’s complement.
    OUT_Z_H_M (05), OUT_Z_L_M (06h)
        Z-axis magnetic field data. The value is expressed as two’s complement.
    OUT_Y_H_M (07), OUT_Y_L_M (08h)
        Y-axis magnetic field data. The value is expressed as two’s complement.

    SR_REG_M
        1: LOCK Data output register lock. Once a new set of measurements is available,
                 this bit is set when the first magnetic file data register has been read.
        0: DRDY Data-ready bit. This bit is when a new set of measurements is available.

    Temp: 8 LSB/deg - 12-bit resolution (in C)
    TEMP_OUT_H_M
        7-0: First byte of temp data. Most significant
    TEMP_OUT_L_M
        7-4: four LSBs of temp value
*/


// Private functions to get I2C data
ret_t LSM303DLHC_accel_getData(int32_t * data_x_ptr, int32_t * data_y_ptr, int32_t * data_z_ptr);
ret_t LSM303DLHC_mag_getData(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);

// Private functions to put data onto our queues
ret_t LSM303DLHC_accel_putPacket(int32_t * data_x_ptr, int32_t * data_y_ptr, int32_t * data_z_ptr);
ret_t LSM303DLHC_mag_putPacket(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);


typedef struct {
    uint8_t head_ind;
    uint8_t tail_ind;
    uint8_t unread_items;
    uint8_t overwrite_count;
} queue_t;


typedef struct {
    uint32_t accel_hw_overwrite_count; //!< Accel overwrites on the chip itself
    uint32_t mag_hw_overwrite_count;   //!< Mag overwrites on the chip itself
    int32_t accel_x_queue[ACCEL_QUEUE_SIZE];  //!< Accel queue to store X packets
    int32_t accel_y_queue[ACCEL_QUEUE_SIZE];  //!< Accel queue to store Y packets
    int32_t accel_z_queue[ACCEL_QUEUE_SIZE];  //!< Accel queue to store Z packets
    float mag_x_queue[MAG_QUEUE_SIZE];        //!< Mag queue to store X packets
    float mag_y_queue[MAG_QUEUE_SIZE];        //!< Mag queue to store Y packets
    float mag_z_queue[MAG_QUEUE_SIZE];        //!< Mag queue to store Z packets
    queue_t accel_queue_admin;         //!< Accel admin struct of the accel_queues
    queue_t mag_queue_admin;           //!< Mag admin struct of the mag_queues
    uint8_t accel_hw_data_available;   //!< Flag we set when we see there's data acording to the status register
    uint8_t mag_hw_data_available;     //!< Flag we set when we see there's data acording to MAG_SR_REG_M
    accel_ODR_t accel_datarate;        //!< Rate at which accel data gets updated on chip
    mag_ODR_t mag_datarate;            //!< Rate at which mag data gets updated on chip
    accel_sensitivity_t accel_sensitivity;  //!< Accel fullscale setting
    mag_sensitivity_t mag_sensitivity;      //!< Accel fullscale setting
} LSM303DLHC_admin_t;


//! Depending on how what gain we set, normalize the values we get out
float MagGainOffsets[] = {1.3f, 1.9f, 2.5f, 4.0f, 4.7f, 5.6f, 8.1f};

LSM303DLHC_admin_t LSM303DLHC_admin;



ret_t LSM303DLHC_init(accel_ODR_t accel_datarate, accel_sensitivity_t accel_sensitivity,
                      mag_ODR_t mag_datarate, mag_sensitivity_t mag_sensitivity)
{
    ret_t retval;
    // Set datarate, enable X, Y, and Z access and turn off Low Power Mode
    retval = I2C_writeByte(ACCEL_ADDRESS, ACCEL_CTRL_REG1_A, (accel_datarate << 4) | (0b0111), true);
    if (retval != RET_OK) { return retval; }

    // Set accel sensitivity & turn on high res mode
    retval = I2C_writeByte(ACCEL_ADDRESS, ACCEL_CTRL_REG4_A, (accel_sensitivity << 4) | 0b00001000, true);
    if (retval != RET_OK) { return retval; }

    // Set the datarate
    retval = I2C_writeByte(MAG_ADDRESS, MAG_CRA_REG_M, (mag_datarate << 2) & 0b10011100, true);
    if (retval != RET_OK) { return retval; }

    // Set the sensitivity
    retval = I2C_writeByte(MAG_ADDRESS, MAG_CRB_REG_M, (mag_sensitivity << 5) & 0b11100000, true);
    if (retval != RET_OK) { return retval; }

    // enable continuous conversion mode
    retval = I2C_writeByte(MAG_ADDRESS, MAG_MR_REG_M, 0x00, true);
    if (retval != RET_OK) { return retval; }

    LSM303DLHC_admin.accel_datarate = accel_datarate;
    LSM303DLHC_admin.accel_sensitivity = accel_sensitivity;
    LSM303DLHC_admin.mag_datarate = mag_datarate;
    LSM303DLHC_admin.mag_sensitivity = mag_sensitivity;
    LSM303DLHC_admin.accel_hw_overwrite_count = 0;
    LSM303DLHC_admin.mag_hw_overwrite_count = 0;

    // we've initialized properly!
    return RET_OK;
}

/* ----------- Functions to get data from chip (I2C transactions) ----------- */

ret_t LSM303DLHC_accel_getData(int32_t * data_x_ptr, int32_t * data_y_ptr, int32_t * data_z_ptr)
{
    int32_t temp_data;
    uint8_t bytes[6];
    uint8_t i;
    ret_t retval;

    // clear all variables
    for (i = 0; i < 6; i++) {
        bytes[i] = 0;
    }
    *data_x_ptr = 0;
    *data_y_ptr = 0;
    *data_z_ptr = 0;

    // read the data starting at register 1
    retval = I2C_readData(ACCEL_ADDRESS, ACCEL_OUT_X_L_A, bytes, 6);
    if (retval == RET_OK) {
        // Build the data up and normalize!
        temp_data = (int32_t)((bytes[0] << 8) | (bytes[1])) >> LSM303DLHC_admin.accel_sensitivity;
        *data_x_ptr = (int32_t)temp_data;

        temp_data = (int32_t)((bytes[2] << 8) | (bytes[3])) >> LSM303DLHC_admin.accel_sensitivity;
        *data_y_ptr = (int32_t)temp_data;

        temp_data = (int32_t)((bytes[4] << 8) | (bytes[5])) >> LSM303DLHC_admin.accel_sensitivity;
        *data_z_ptr = (int32_t)temp_data;

        return RET_OK;
    } else {
        // Raise some sort of error...
        return RET_COM_ERR;
    }
}

ret_t LSM303DLHC_mag_getData(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
{
    int16_t temp_data;
    uint8_t bytes[6];
    uint8_t i;
    ret_t retval;

    // clear all variables
    for (i = 0; i < 6; i++) {
        bytes[i] = 0;
    }
    *data_x_ptr = 0;
    *data_y_ptr = 0;
    *data_z_ptr = 0;

    // read the data starting at register 1
    retval = I2C_readData(MAG_ADDRESS, MAG_OUT_X_H_M, bytes, 6);
    if (retval == RET_OK) {
        // Build the data up and normalize!
        // TODO: This is really inefficient math...
        temp_data = (int16_t)(bytes[0] << 8) | (bytes[1]);
        *data_x_ptr = ((float)(temp_data)) / MagGainOffsets[LSM303DLHC_admin.mag_sensitivity - 1];
        temp_data = (int16_t)(bytes[2] << 8) | (bytes[3]);
        *data_y_ptr = ((float)(temp_data)) / MagGainOffsets[LSM303DLHC_admin.mag_sensitivity - 1];
        temp_data = (int16_t)(bytes[4] << 8) | (bytes[5]);
        *data_z_ptr = ((float)(temp_data)) / MagGainOffsets[LSM303DLHC_admin.mag_sensitivity - 1];
        return RET_OK;
    } else {
        // Raise some sort of error...
        return RET_COM_ERR;
    }
}

ret_t LSM303DLHC_temp_getData(int16_t * temp_val_ptr)
{
    ret_t retval;
    uint8_t bytes[] = {0, 0};

    // read out the data
    retval = I2C_readData(MAG_ADDRESS, MAG_TEMP_OUT_H_M, bytes, 2);
    if (retval != RET_OK) { return retval; }

    // build up value!
    *temp_val_ptr = (bytes[0] << 8 | (bytes[1] & 0xF0));

    return RET_OK;
}

ret_t LSM303DLHC_checkStatus(void)
{
    uint8_t data;
    ret_t retval;

    retval = I2C_readData(ACCEL_ADDRESS, ACCEL_STATUS_REG_A, &data, 1);
    if (retval != RET_OK) { return retval; }

    if (data & 0xF0) {
        // Some sort of data overwrite
        LSM303DLHC_admin.accel_hw_overwrite_count += 1;
    }
    if (data & 0x08) {
        // data available
        LSM303DLHC_admin.accel_hw_data_available = 1;
    }

    retval = I2C_readData(MAG_ADDRESS, MAG_SR_REG_M, &data, 1);
    if (retval != RET_OK) { return retval; }

    if (data & 0x01) {
        // data available
        LSM303DLHC_admin.mag_hw_data_available = 1;
    }

    return RET_OK;
}


/* ----------- Interrupt handlers (Get data from chip when ready) ----------- */


/*! Interrupt handler for the DRDY interrupt pin coming from this chip.
 *
 */
void LSM303DLHC_drdy_handler(void)
{
    ret_t retval;
    int32_t a_data_x;
    int32_t a_data_y;
    int32_t a_data_z;

    float m_data_x;
    float m_data_y;
    float m_data_z;

    retval = LSM303DLHC_checkStatus();
    if (retval == RET_OK) {
        // check if we have accel packets to get
        if (LSM303DLHC_admin.accel_hw_data_available != 0) {
            LSM303DLHC_admin.accel_hw_data_available = 0;
            retval = LSM303DLHC_accel_getData(&a_data_x, &a_data_y, &a_data_z);
            if (retval == RET_OK) {
                retval = LSM303DLHC_accel_putPacket(&a_data_x, &a_data_y, &a_data_z);
            }
        }
        // check if we have mag packets to get
        if (LSM303DLHC_admin.mag_hw_data_available != 0) {
            LSM303DLHC_admin.mag_hw_data_available = 0;
            retval = LSM303DLHC_mag_getData(&m_data_x, &m_data_y, &m_data_z);
            if (retval == RET_OK) {
                retval = LSM303DLHC_mag_putPacket(&m_data_x, &m_data_y, &m_data_z);
            }
        }
    }

}

// /*! Interrupt handler for the INT1 interrupt pin coming from this chip.
//  *
//  */
// // TODO: Implement LSM303DLHC INT1 handler
// void LSM303DLHC_int_handler(void)
// {
//     return;
// }


/* -------------- Functions to set/get data to/from our queues -------------- */


/*! Puts the given data onto the accel queue.
 *
 * Note: This is the ONLY function that modifies the head index of the accel queue.
 *   It also can modify the tail index if we're overwritting unread data.
 */
ret_t LSM303DLHC_accel_putPacket(int32_t * data_x_ptr, int32_t * data_y_ptr, int32_t * data_z_ptr)
{
    // Get the data from the queue...
    LSM303DLHC_admin.accel_x_queue[LSM303DLHC_admin.accel_queue_admin.head_ind] = *data_x_ptr;
    LSM303DLHC_admin.accel_y_queue[LSM303DLHC_admin.accel_queue_admin.head_ind] = *data_y_ptr;
    LSM303DLHC_admin.accel_z_queue[LSM303DLHC_admin.accel_queue_admin.head_ind] = *data_z_ptr;

    // the queue didn't change while we were getting it!

    // now check if we're about to overwrite data
    if (LSM303DLHC_admin.accel_queue_admin.unread_items == ACCEL_QUEUE_SIZE) {
        // if we're overwriting data, we need to move the tail too
        if (LSM303DLHC_admin.accel_queue_admin.tail_ind < ACCEL_QUEUE_SIZE - 1) {
            LSM303DLHC_admin.accel_queue_admin.tail_ind += 1;
        } else {
            LSM303DLHC_admin.accel_queue_admin.tail_ind = 0;
        }
        // the number of unread items stays the same, but increment overwrite counter
        LSM303DLHC_admin.accel_queue_admin.overwrite_count += 1;
    } else {
        LSM303DLHC_admin.accel_queue_admin.unread_items += 1;
    }
    // move the head forward either way
    if (LSM303DLHC_admin.accel_queue_admin.head_ind < ACCEL_QUEUE_SIZE - 1) {
        LSM303DLHC_admin.accel_queue_admin.head_ind += 1;
    } else {
        LSM303DLHC_admin.accel_queue_admin.head_ind = 0;
    }
    return RET_OK;
}

/*! Puts the given data onto the mag queue.
 *
 * Note: This is the ONLY function that modifies the head index of the mag queue.
 *   It also can modify the tail index if we're overwritting unread data.
 */
ret_t LSM303DLHC_mag_putPacket(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
{

    // Put the data on the queue...
    LSM303DLHC_admin.mag_x_queue[LSM303DLHC_admin.mag_queue_admin.head_ind] = *data_x_ptr;
    LSM303DLHC_admin.mag_y_queue[LSM303DLHC_admin.mag_queue_admin.head_ind] = *data_y_ptr;
    LSM303DLHC_admin.mag_z_queue[LSM303DLHC_admin.mag_queue_admin.head_ind] = *data_z_ptr;

    // now check if we've overwritten data
    if (LSM303DLHC_admin.mag_queue_admin.unread_items == MAG_QUEUE_SIZE) {
        // if we're overwriting data, we need to move the tail too
        if (LSM303DLHC_admin.mag_queue_admin.tail_ind < MAG_QUEUE_SIZE - 1) {
            LSM303DLHC_admin.mag_queue_admin.tail_ind += 1;
        } else {
            LSM303DLHC_admin.mag_queue_admin.tail_ind = 0;
        }
        // the number of unread items stays the same, but increment overwrite counter
        LSM303DLHC_admin.mag_queue_admin.overwrite_count += 1;
    } else {
        LSM303DLHC_admin.mag_queue_admin.unread_items += 1;
    }
    // move the head forward either way
    if (LSM303DLHC_admin.mag_queue_admin.head_ind < MAG_QUEUE_SIZE - 1) {
        LSM303DLHC_admin.mag_queue_admin.head_ind += 1;
    } else {
        LSM303DLHC_admin.mag_queue_admin.head_ind = 0;
    }

    return RET_OK;
}

/*! Gets data from the accel queue.
 *
 * Note: This is function modifies the tail index if we're overwritting unread data.
 *   LSM303DLHC_accel_putPacket also can modify it, so we must check for that case in
 *   this function as LSM303DLHC_accel_getPacket gets called asynchronously.
 */
ret_t LSM303DLHC_accel_getPacket(int32_t * data_x_ptr, int32_t * data_y_ptr, int32_t * data_z_ptr)
{
    uint8_t queue_ind = LSM303DLHC_admin.accel_queue_admin.tail_ind;
    if ( LSM303DLHC_accel_dataAvailable() ) {

        // Get the data from the queue...
        *data_x_ptr = LSM303DLHC_admin.accel_x_queue[queue_ind];
        *data_y_ptr = LSM303DLHC_admin.accel_y_queue[queue_ind];
        *data_z_ptr = LSM303DLHC_admin.accel_z_queue[queue_ind];

        // Check if this data is valid (could've been overwritten mid way through)
        if (queue_ind != LSM303DLHC_admin.accel_queue_admin.tail_ind) {
            return RET_COM_ERR;
        } else {
            // the queue didn't change while we were getting it!
            LSM303DLHC_admin.accel_queue_admin.unread_items -= 1;
            if (LSM303DLHC_admin.accel_queue_admin.tail_ind < ACCEL_QUEUE_SIZE - 1) {
                LSM303DLHC_admin.accel_queue_admin.tail_ind += 1;
            } else {
                LSM303DLHC_admin.accel_queue_admin.tail_ind = 0;
            }
            return RET_OK;
        }
    } else {

        return RET_NODATA_ERR;
    }
}

/*! Gets data from the mag queue.
 *
 * Note: This is function modifies the tail index if we're overwritting unread data.
 *   LSM303DLHC_mag_putPacket also can modify it, so we must check for that case in
 *   this function as LSM303DLHC_mag_getPacket gets called asynchronously.
 */
ret_t LSM303DLHC_mag_getPacket(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
{
    uint8_t queue_ind = LSM303DLHC_admin.mag_queue_admin.tail_ind;
    if ( LSM303DLHC_mag_dataAvailable() ) {

        // Get the data from the queue...
        *data_x_ptr = LSM303DLHC_admin.mag_x_queue[queue_ind];
        *data_y_ptr = LSM303DLHC_admin.mag_y_queue[queue_ind];
        *data_z_ptr = LSM303DLHC_admin.mag_z_queue[queue_ind];

        // Check if this data is valid (could've been overwritten mid way through)
        if (queue_ind != LSM303DLHC_admin.mag_queue_admin.tail_ind) {
            return RET_COM_ERR;
        } else {
            // the queue didn't change while we were getting it!
            LSM303DLHC_admin.mag_queue_admin.unread_items -= 1;
            if (LSM303DLHC_admin.mag_queue_admin.tail_ind < MAG_QUEUE_SIZE - 1) {
                LSM303DLHC_admin.mag_queue_admin.tail_ind += 1;
            } else {
                LSM303DLHC_admin.mag_queue_admin.tail_ind = 0;
            }
            return RET_OK;
        }
    } else {

        return RET_NODATA_ERR;
    }
}

/*! Checks if there is data available on our accel queue.
 *
 */
bool LSM303DLHC_accel_dataAvailable(void)
{
    if (LSM303DLHC_admin.accel_queue_admin.unread_items == 0) {
        return false;
    } else {
        return true;
    }
}

/*! Checks if there is data available on our mag queue.
 *
 */
bool LSM303DLHC_mag_dataAvailable(void)
{
    if (LSM303DLHC_admin.mag_queue_admin.unread_items == 0) {
        return false;
    } else {
        return true;
    }
}
