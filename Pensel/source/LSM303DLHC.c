/*
 *
 *
 */

#include <stdint.h>
#include "common.h"
#include "I2C.h"
#include "LSM303DLHC.h"


#define ACCEL_ADDRESS (0b0011001)
#define MAG_ADDRESS   (0b0011110)

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

// CTRL_REG1_A
//     7-4: ODR[3:0]   Data rate selection. Default value: 0000 (0000: power-down, others: refer to Table 20)
//     3: LPen         Low-power mode enable. Default value: 0 (0: normal mode, 1: low-power mode)
//     2: Zen          Z-axis enable. Default value: 1   (0: Z-axis disabled, 1: Z-axis enabled)
//     1: Yen          Y-axis enable. Default value: 1   (0: Y-axis disabled, 1: Y-axis enabled)
//     0: Xen          X-axis enable. Default value: 1   (0: X-axis disabled, 1: X-axis enabled)

// CTRL_REG2_A
//     7-6: HPM[1:0]   High-pass filter mode selection. Default value: 00 (refer to Table 23)
//     5-4: HPCF[2:1]  High-pass filter cutoff frequency selection
//     3:   FDS        Filtered data selection. Default value: 0   (0: internal filter bypassed, 1: data from internal filter sent to output register and FIFO)
//     2:   HPCLICK    High-pass filter enabled for click function (0: filter bypassed, 1: filter enabled)
//     1:   HPIS2      High-pass filter enabled for AOI function on Interrupt 2 (0: filter bypassed, 1: filter enabled)
//     0:   HPIS1      High-pass filter enabled for AOI function on Interrupt 1 (0: filter bypassed, 1: filter enabled)

// CTRL_REG3_A
//     7: I1_CLICK   CLICK interrupt on INT1. Default value 0 (0: disable, 1: enable)
//     6: I1_AOI1    AOI1 interrupt on INT1. Default value 0 (0: disable, 1: enable)
//     5: I1_AOI2    AOI2 interrupt on INT1. Default value 0 (0: disable, 1: enable)
//     4: I1_DRDY1   DRDY1 interrupt on INT1. Default value 0 (0: disable, 1: enable)
//     3: I1_DRDY2   DRDY2 interrupt on INT1. Default value 0 (0: disable, 1: enable)
//     2: I1_WTM     FIFO watermark interrupt on INT1. Default value 0 (0: disable, 1: enable)
//     1: I1_OVERRUN FIFO overrun interrupt on INT1. Default value 0 (0: disable, 1: enable)

// CTRL_REG4_A
//     7:   BDU      Block data update. Default value: 0   (0: continuous update, 1: output registers not updated until MSB and LSB have been read
//     6:   BLE      Big/little endian data selection. Default value 0.   (0: data LSB @ lower address, 1: data MSB @ lower address)
//     5-4: FS[1:0]  Full-scale selection. Default value: 00 (00: ±2 g, 01: ±4 g, 10: ±8 g, 11: ±16 g)
//     3:   HR       High-resolution output mode: Default value: 0   (0: high-resolution disable, 1: high-resolution enable)
//     1-2: ZERO     (MUST ALWAYS BE SET TO ZERO)
//     0:   SIM      SPI serial interface mode selection. Default value: 0 (0: 4-wire interface, 1: 3-wire interface).

// CTRL_REG5_A
//     7:   BOOT       Reboot memory content. Default value: 0 (0: normal mode, 1: reboot memory content)
//     6:   FIFO_EN    FIFO enable. Default value: 0   (0: FIFO disable, 1: FIFO enable)
//     5-4: DONTCARE
//     3:   LIR_INT1   Latch interrupt request on INT1_SRC register, with INT1_SRC register cleared by reading INT1_SRC itself.
//                          Default value: 0.   (0: interrupt request not latched, 1: interrupt request latched)
//     2:   D4D_INT1   4D enable: 4D detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1.
//     1:   LIR_INT2   Latch interrupt request on INT2_SRC register, with INT2_SRC register cleared by reading INT2_SRC itself.
//                          Default value: 0.   (0: interrupt request not latched, 1: interrupt request latched)
//     0:   D4D_INT2   4D enable: 4D detection is enabled on INT2 when 6D bit on INT2_CFG is set to 1.

// CTRL_REG6_A
//     7:    I2_CLICKen   CLICK interrupt on PAD2. Default value 0. (0: disable, 1: enable)
//     6:    I2_INT1      Interrupt 1 on PAD2. Default value 0. (0: disable, 1: enable)
//     5:    I2_INT2      Interrupt 2 on PAD2. Default value 0. (0: disable, 1: enable)
//     4:    BOOT_I1      Reboot memory content on PAD2. Default value: 0 (0: disable, 1: enable)
//     3:    P2_ACT       Active function status on PAD2. Default value 0 (0: disable, 1: enable)
//     2, 0: DONTCARE
//     1:    H_LACTIVE    Interrupt active high, low. Default value 0. (0: active high, 1: active low)

// REFERENCE_A
//     7-0: Ref[7:0]  Reference value for interrupt generation. Default value: 0000 0000

// STATUS_A
//     7: ZYXOR   X-, Y-, and Z-axis data overrun.
//     6: ZOR     Z-axis data overrun.
//     5: YOR     Y-axis data overrun.
//     4: XOR     X-axis data overrun.
//     3: ZYXDA   X-, Y-, and Z-axis new data available.
//     2: ZDA     Z-axis new data available.
//     1: YDA     Y-axis new data available.
//     0: XDA     X-axis new data available.

// OUT_X_L_A (28h), OUT_X_H_A (29h)
//     X-axis acceleration data. The value is expressed in two’s complement.
// OUT_Y_L_A (2Ah), OUT_Y_H_A (2Bh)
//     Y-axis acceleration data. The value is expressed in two’s complement.
// OUT_Z_L_A (2Ch), OUT_Z_H_A (2Dh)
//     Z-axis acceleration data. The value is expressed in two’s complement.


// TODO: Actually figure out how best to init this guy..
ret_t LSM303DLHC_init(accel_ODR_t datarate)
{
    ret_t retval;
    // Set datarate, enable X,Y, and Z access and turn off Low Power Mode
    retval = I2C_write_byte(ACCEL_ADDRESS, ACCEL_CTRL_REG1_A, (datarate << 4) | (0b0111));
    if (retval != RET_OK) { return retval; }

    retval = I2C_write_byte(ACCEL_ADDRESS, ACCEL_CTRL_REG2_A, 0x01);
    if (retval != RET_OK) { return retval; }

    retval = I2C_write_byte(MAG_ADDRESS, MAG_CRA_REG_M, 0x01);
    if (retval != RET_OK) { return retval; }

    retval = I2C_write_byte(MAG_ADDRESS, MAG_CRB_REG_M, 0x01);
    if (retval != RET_OK) { return retval; }

    // we've initialized properly!
    return RET_OK;
}


ret_t LSM303DLHC_accel_getdata(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
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
    retval = I2C_read_bytes(ACCEL_ADDRESS, ACCEL_OUT_X_L_A, 6, bytes);
    if (retval == RET_OK) {
        // Build the data up and normalize!
        // Note: Divide should be simplified to bitshift or multiply by reciprical
        temp_data = (int16_t)(bytes[0] << 8) | (bytes[1]);
        *data_x_ptr = (float)(temp_data);
        temp_data = (int16_t)(bytes[2] << 8) | (bytes[3]);
        *data_y_ptr = (float)(temp_data);
        temp_data = (int16_t)(bytes[4] << 8) | (bytes[5]);
        *data_z_ptr = (float)(temp_data);
        return RET_OK;
    } else {
        // Raise some sort of error...
        return RET_COM_ERR;
    }
}



ret_t LSM303DLHC_mag_getdata(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
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
    retval = I2C_read_bytes(MAG_ADDRESS, MAG_OUT_X_H_M, 6, bytes);
    if (retval == RET_OK) {
        // Build the data up and normalize!
        // Note: Divide should be simplified to bitshift or multiply by reciprical
        temp_data = (int16_t)(bytes[0] << 8) | (bytes[1]);
        *data_x_ptr = (float)(temp_data);
        temp_data = (int16_t)(bytes[2] << 8) | (bytes[3]);
        *data_y_ptr = (float)(temp_data);
        temp_data = (int16_t)(bytes[4] << 8) | (bytes[5]);
        *data_z_ptr = (float)(temp_data);
        return RET_OK;
    } else {
        // Raise some sort of error...
        return RET_COM_ERR;
    }
}
