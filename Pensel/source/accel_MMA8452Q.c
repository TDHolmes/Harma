/*
 *
 *
 */

#include <stdint.h>
#include "common.h"
#include "I2C.h"
#include "accel_MMA8452Q.h"


#define MMA8452Q_REG_X_DATA_MSB  (1)
#define MMA8452Q_REG_X_DATA_LSB  (2)
#define MMA8452Q_REG_Y_DATA_MSB  (3)
#define MMA8452Q_REG_Y_DATA_LSB  (4)
#define MMA8452Q_REG_Z_DATA_MSB  (5)
#define MMA8452Q_REG_Z_DATA_LSB  (6)

#define MMA8452Q_REG_CTRL_REG1 (0x2A)
#define MMA8452Q_REG_CTRL_REG2 (0x2B)
#define MMA8452Q_REG_CTRL_REG3 (0x2C)
#define MMA8452Q_REG_CTRL_REG4 (0x2D)
#define MMA8452Q_REG_CTRL_REG5 (0x2E)

#define FULLSCALE_GS (2) //! The fullscale of the sensor is 2 g's


ret_t accel_init(uint8_t accel_address)
{
    ret_t retval;
    // 0x2A -> 0b00011001 (100 Hz data rate, active mode)
    retval = I2C_write_byte(accel_address, MMA8452Q_REG_CTRL_REG1, 0b00011001);
    if (retval != RET_OK) { return retval; }

    // 0x2B -> 0b00011010 (disable auto sleep, enable high resolution mode)
    retval = I2C_write_byte(accel_address, MMA8452Q_REG_CTRL_REG2, 0b00011010);
    if (retval != RET_OK) { return retval; }

    // 0x2D -> 0x01 (enable data ready interrupt)
    retval = I2C_write_byte(accel_address, MMA8452Q_REG_CTRL_REG1, 0x01);
    if (retval != RET_OK) { return retval; }

    // 0x2E -> 0x01 (route data ready interrupt to INT1 pin)
    retval = I2C_write_byte(accel_address, MMA8452Q_REG_CTRL_REG1, 0x01);
    if (retval != RET_OK) { return retval; }
    return RET_OK;
}


ret_t accel_getdata(uint8_t accel_address, float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
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
    retval = I2C_read_bytes(accel_address, MMA8452Q_REG_X_DATA_MSB, 6, bytes);
    if (retval == RET_OK) {
        // Build the data up and normalize!
        temp_data = (bytes[0] << 8) | (bytes[1]);
        *data_x_ptr = (float)(temp_data / FULLSCALE_GS);  // Divide should be simplified to multiply by reciprical by compiler
        temp_data = (bytes[2] << 8) | (bytes[3]);
        *data_y_ptr = (float)(temp_data / FULLSCALE_GS);
        temp_data = (bytes[4] << 8) | (bytes[5]);
        *data_z_ptr = (float)(temp_data / FULLSCALE_GS);
        return RET_OK;
    } else {
        // Raise some sort of error...
        return RET_COM_ERR;
    }
}


/*
Register Mapping and bit fields:

REGISTER 0x00: STATUS
    ZYXOW (7): X, Y, Z-axis data overwrite.
    ZOW   (6): Z-axis data overwrite.
    YOW   (5): Y-axis data overwrite.
    XOW   (4): X-axis data overwrite.
    ZYXDR (3): X, Y, Z-axis new data ready.
    ZDR   (2): Z-axis new data available.
    YDR   (1): Y-axis new data available.
    XDR   (0): X-axis new data available.

REGISTER 0x01: OUT_X_MSB: X_MSB register (bits 11-4)
REGISTER 0x02: OUT_X_LSB: X_LSB register (bits 3-0, upper nibble)
REGISTER 0x03: OUT_Y_MSB: Y_MSB register (bits 11-4)
REGISTER 0x04: OUT_Y_LSB: Y_LSB register (bits 3-0, upper nibble)
REGISTER 0x05: OUT_Z_MSB: Z_MSB register (bits 11-4)
REGISTER 0x06: OUT_Z_LSB: Z_LSB register (bits 3-0, upper nibble)

REGISTER 0x0B: SYSMOD system mode register (Default value: 00)
    00: Standby mode
    01: Wake mode
    10: Sleep mode

REGISTER 0x0C: INT_SOURCE system interrupt status register
    SRC_ASLP   (bit 7): Logic ‘1’ indicates that an interrupt event that can cause a wake to sleep or
                        sleep to wake system mode transition has occurred
    SRC_TRANS  (bit 5): This bit is asserted whenever EA bit in the TRANS_SRC is asserted and the interrupt has been enabled.
    SRC_LNDPRT (bit 4): This bit is asserted whenever NEWLP bit in the PL_STATUS is asserted and the interrupt has been enabled.
    SRC_PULSE  (bit 3): This bit is asserted whenever EA bit in the PULSE_SRC is asserted and the interrupt has been enabled.
    SRC_FF_MT  (bit 2): Freefall/motion detected
    SRC_DRDY   (bit 0): Data Ready

REGISTER 0x0E: XYZ_DATA_CFG register
    HPF_OUT0 (bit 4): Enable the high pass filter [disabled by default]
    FS1      (bit 1): Full scale selection (1st bit)
    FS0      (bit 0): Full scale selection (0th bit)
        00: 2g [default]
        01: 4g
        10: 8g

REGISTER 0x0F: HP_FILTER_CUTOFF high-pass filter register
    Pulse_HPF_BYP (bit 5): Bypass high-pass filter for pulse processing function
    Pulse_LPF_EN  (bit 4): Enable low-pass filter for pulse processing function.
    SEL1          (bit 1): HPF cutoff frequency selection
    SEL0          (bit 0): HPF cutoff frequency selection (refer to Table 18.)

REGISTER 0x2A: CTRL_REG1 system control 1 register
    ASLP_RATE1 (bit 7): Auto Sleep mode rate. Default value: 00 (50 Hz)
    ASLP_RATE0 (bit 6): See Table 54 for more data rates
    DR2        (bit 5): Data rate selection. Default value: 000 (800 Hz)
    DR1        (bit 4): See Table 55 for more data rates
    DR0        (bit 3):
    LNOISE     (bit 2): Reduced noise reduced maximum range mode (0: Normal mode; 1: Reduced noise mode)
    F_READ     (bit 1): Fast read (limit data to 8 bits)
    ACTIVE     (bit 0): Full-scale selection. (0: Standby mode; 1: Active mode)

REGISTER 0x2B: CTRL_REG2 system control 2 register
    ST     (bit 7): Enable self test
    RST    (bit 6): Software reset
    SMODS1 (bit 4): Sleep mode power scheme selection. (11 for Low Power)
    SMODS0 (bit 3): See table 58 and 59
    SLPE   (bit 2): Auto-sleep enable.
    MODS1  (bit 1): Active mode power scheme selection. (10 for High Resolution)
    MODS0  (bit 0): See table 58 and 59

0x2C: CTRL_REG3 interrupt control register
    WAKE_TRANS  (bit 6): Allow transient to cause an interrupt
    WAKE_LNDPRT (bit 5): Allow landscape/portrait to cause an interrupt
    WAKE_PULSE  (bit 4): Allow pulse detection to cause an interrupt
    WAKE_FF_MT  (bit 3): Allow freefall to cause an interrupt
    IPOL        (bit 1): Interrupt polarity. Defaults to active low
    PP_OD       (bit 0): Interrupt pin type. 0: Push-pull   1: Open drain

0x2D: CTRL_REG4 register
    INT_EN_ASLP   (bit 7): Auto-sleep/wake interrupt
    INT_EN_TRANS  (bit 5): Transient interrupt
    INT_EN_LNDPRT (bit 4): Orientation (landscape/portrait) interrupt
    INT_EN_PULSE  (bit 3): Pulse detection interrupt
    INT_EN_FF_MT  (bit 2): Freefall/motion interrupt
    INT_EN_DRDY   (bit 0): Data-ready interrupt

0x2E: CTRL_REG5 register
    INT_CFG_ASLP   (bit n): For Auto-sleep,           0: Int -> INT2 pin;   1: Int -> INT1 pin
    INT_CFG_TRANS  (bit n): For transient interrupt,  0: Int -> INT2 pin;   1: Int -> INT1 pin
    INT_CFG_LNDPRT (bit n): For landscape interrupt,  0: Int -> INT2 pin;   1: Int -> INT1 pin
    INT_CFG_PULSE  (bit n): For pulse interrupt,      0: Int -> INT2 pin;   1: Int -> INT1 pin
    INT_CFG_FF_MT  (bit n): For free-fall interrupt,  0: Int -> INT2 pin;   1: Int -> INT1 pin
    INT_CFG_DRDY   (bit n): For data ready interrupt, 0: Int -> INT2 pin;   1: Int -> INT1 pin

0x2F: OFF_X offset correction X register
0x30: OFF_Y offset correction Y register
0x31: OFF_Z offset correction Z register

USELESS REGISTERS
    0x10: PL_STATUS portrait/landscape status register
    0x11: Portrait/landscape configuration register
    0x12: Portrait/landscape debounce counter
    0x13: PL_BF_ZCOMP back/front and Z compensation register
    0x14: PL_THS_REG portrait/landscape threshold and hysteresis register
    0x15: FF_MT_CFG freefall/motion configuration register
    0x16: FF_MT_SRC freefall/motion source register
    0x17: FF_MT_THS freefall and motion threshold register
    0x18: FF_MT_COUNT debounce register
    0x1D: Transient_CFG register
*/
