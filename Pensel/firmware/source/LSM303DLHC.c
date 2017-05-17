/*
 *
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "queue.c"
#include "hardware.h"
#include "I2C.h"
#include "LSM303DLHC.h"
#include "Drivers/stm32f3xx_hal.h"


#define ACCEL_ADDRESS (0b00110010)
#define MAG_ADDRESS   (0b00111100)

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


// Private functions to get I2C data
ret_t LSM303DLHC_accel_getData(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);
ret_t LSM303DLHC_mag_getData(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);

// Private functions to put data onto our queues
ret_t LSM303DLHC_accel_putPacket(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);
ret_t LSM303DLHC_mag_putPacket(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);


typedef struct {
    uint32_t accel_hw_overwrite_count; //!< Accel overwrites on the chip itself
    uint32_t mag_hw_overwrite_count;   //!< Mag overwrites on the chip itself
    accel_packet_t accel_pkts[ACCEL_QUEUE_SIZE];  //!< Accel queue to store X packets
    mag_packet_t mag_pkts[MAG_QUEUE_SIZE];        //!< Mag queue to store packets
    queue_t accel_queue;            //!< Accel admin struct of the accel_queues
    queue_t mag_queue;              //!< Mag admin struct of the mag_queues
    uint8_t accel_data_available;   //!< Flag we set when we see there's data acording to the status register
    uint8_t mag_data_available;     //!< Flag we set when we see there's data acording to MAG_SR_REG_M
    accel_ODR_t accel_datarate;     //!< Rate at which accel data gets updated on chip
    mag_ODR_t mag_datarate;         //!< Rate at which mag data gets updated on chip
    accel_sensitivity_t accel_sensitivity;  //!< Accel fullscale setting
    mag_sensitivity_t mag_sensitivity;      //!< Accel fullscale setting
} LSM303DLHC_admin_t;


//! Mag gain offsets for the X & Y axis to normalize to miligauss
static float MagGainOffsets_XY[] = {0.9091f, 1.1696f, 1.4925f, 2.2222f, 2.5000f, 3.0303f, 4.3478f};
//! Mag gain offsets for the Z axis to normalize to miligauss
static float MagGainOffsets_Z[] = {1.0204f, 1.3158f, 1.6667f, 2.5000f, 2.8169f, 3.3898f, 4.8780f};

//! Accel gain offsets to normalize to mili-g (g = 9.8 m/s^2)
static float AccelGainOffsets[] = {1.0f, 2.0f, 4.0f, 12.0f};

/*! Master struct for this module. Marked as volatile as we update it in
 *   the I2C & DRDY pin interrupt context */
volatile LSM303DLHC_admin_t LSM303DLHC;


/*! Initializer of the LSM303DLHC module and chip. Sets the datarate and sensitivity of the
 *    magnetometer and accelerometer.
 *
 * @param accel_datarate (accel_ODR_t): Datarate at which the accelerometer module will produce data.
 * @param accel_sensitivity (accel_sensitivity_t): Fullscale sensitivity of the accel module.
 * @param mag_datarate (mag_ODR_t): Datarate at which the magnetometer module will produce data.
 * @param mag_sensitivity (mag_sensitivity_t): Fullscale sensitivity of the magnetometer module.
 * @retval (ret_t): Success or failure of initializing the LSM303DLHC module.
 */
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

    LSM303DLHC.accel_datarate = accel_datarate;
    LSM303DLHC.accel_sensitivity = accel_sensitivity;
    LSM303DLHC.mag_datarate = mag_datarate;
    LSM303DLHC.mag_sensitivity = mag_sensitivity;
    LSM303DLHC.accel_hw_overwrite_count = 0;
    LSM303DLHC.mag_hw_overwrite_count = 0;

    // we've initialized properly!
    return RET_OK;
}

/* ----------- Functions to get data from chip (I2C transactions) ----------- */

/*! Gets the new accel data via I2C.
 *
 * @param data_x_ptr (float *): Location where the data from the x axis should be stored.
 * @param data_y_ptr (float *): Location where the data from the y axis should be stored.
 * @param data_z_ptr (float *): Location where the data from the z axis should be stored.
 * @retval (ret_t): Success or failure of getting the accel packet from the chip via I2C.
 */
ret_t LSM303DLHC_accel_getData(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
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
    retval = I2C_readData(ACCEL_ADDRESS, ACCEL_OUT_X_L_A | 0x80, bytes, 6);
    if (retval == RET_OK) {
        // Build the data up and normalize!
        temp_data = (int16_t)((bytes[1] << 8) | (bytes[0])) >> 4;
        *data_x_ptr = ((float)temp_data) * AccelGainOffsets[LSM303DLHC.accel_sensitivity];

        temp_data = (int16_t)((bytes[3] << 8) | (bytes[2])) >> 4;
        *data_y_ptr = ((float)temp_data) * AccelGainOffsets[LSM303DLHC.accel_sensitivity];

        temp_data = (int16_t)((bytes[5] << 8) | (bytes[4])) >> 4;
        *data_z_ptr = ((float)temp_data) * AccelGainOffsets[LSM303DLHC.accel_sensitivity];

        return RET_OK;
    } else {
        // Raise some sort of error...
        return RET_COM_ERR;
    }
}

/*! Gets the new mag data via I2C.
 *
 * @param data_x_ptr (float *): Location where the data from the x axis should be stored.
 * @param data_y_ptr (float *): Location where the data from the y axis should be stored.
 * @param data_z_ptr (float *): Location where the data from the z axis should be stored.
 * @retval (ret_t): Success or failure of getting the mag packet from the chip via I2C.
 */
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
        temp_data = (int16_t)((bytes[0] << 8) | (bytes[1]));
        *data_x_ptr = ((float)temp_data) * MagGainOffsets_XY[LSM303DLHC.mag_sensitivity - 1];
        temp_data = (int16_t)((bytes[2] << 8) | (bytes[3]));
        *data_y_ptr = ((float)temp_data) * MagGainOffsets_XY[LSM303DLHC.mag_sensitivity - 1];
        temp_data = (int16_t)((bytes[4] << 8) | (bytes[5]));
        *data_z_ptr = ((float)temp_data) * MagGainOffsets_Z[LSM303DLHC.mag_sensitivity - 1];
        return RET_OK;
    } else {
        // Raise some sort of error...
        return RET_COM_ERR;
    }
}

/*! Gets temperature data via I2C.
 *
 * @param temp_val_ptr (int16_t *): Location where the data from the temp sensor should be stored.
 * @retval (ret_t): Success or failure of getting the temp from the chip via I2C.
 */
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

/*! Gets status info from the accel and mag module via I2C and updates the
 *    LSM303DLHC_admin_t struct accordingly.
 *
 * @retval (ret_t): Success or failure of getting the data from the chip via I2C.
 */
ret_t LSM303DLHC_checkStatus(void)
{
    uint8_t data;
    ret_t retval;

    retval = I2C_readData(ACCEL_ADDRESS, ACCEL_STATUS_REG_A, &data, 1);
    if (retval != RET_OK) { return retval; }

    if (data & 0xF0) {
        // Some sort of data overwrite
        LSM303DLHC.accel_hw_overwrite_count += 1;
    }
    if (data & 0x08) {
        // data available
        LSM303DLHC.accel_data_available = 1;
    }

    retval = I2C_readData(MAG_ADDRESS, MAG_SR_REG_M, &data, 1);
    if (retval != RET_OK) { return retval; }

    if (data & 0x01) {
        // data available
        LSM303DLHC.mag_data_available = 1;
    }

    return RET_OK;
}


/* ----------- Interrupt handlers (Get data from chip when ready) ----------- */


/*! Interrupt handler for the DRDY interrupt pin coming from this chip.
 *
 *   1. Check the status register of the chip, update error flags, and check for available data.
 *   2. Based on the status, get the available data.
 *   3. Put that data into a packet queue.
 */
void LSM303DLHC_drdy_handler(void)
{
    ret_t retval;
    float accel_data_x;
    float accel_data_y;
    float accel_data_z;

    float mag_data_x;
    float mag_data_y;
    float mag_data_z;

    retval = LSM303DLHC_checkStatus();
    if (retval == RET_OK) {
        // check if we have accel packets to get
        if (LSM303DLHC.accel_data_available != 0) {
            LSM303DLHC.accel_data_available = 0;
            // TODO: Check round trip latency of getting accel packet & putting it into the queue
            TimingPin_set(1);
            retval = LSM303DLHC_accel_getData(&accel_data_x, &accel_data_y, &accel_data_z);
            if (retval == RET_OK) {
                retval = LSM303DLHC_accel_putPacket(&accel_data_x, &accel_data_y, &accel_data_z);
            }  // else { error!!!; }
            TimingPin_set(0);
        }
        // check if we have mag packets to get
        if (LSM303DLHC.mag_data_available != 0) {
            LSM303DLHC.mag_data_available = 0;
            // TODO: Check round trip latency of getting mag packet & putting it into the queue
            TimingPin_set(1);
            retval = LSM303DLHC_mag_getData(&mag_data_x, &mag_data_y, &mag_data_z);
            if (retval == RET_OK) {
                retval = LSM303DLHC_mag_putPacket(&mag_data_x, &mag_data_y, &mag_data_z);
            }  // else { error!!!; }
            TimingPin_set(0);
        }
    }

}

/* -------------- Functions to set/get data to/from our queues -------------- */


/*! Puts the given data onto the accel queue.
 *
 * Note: This is the ONLY function that modifies the head index of the accel queue.
 *   It also can modify the tail index if we're overwritting unread data.
 *
 * @param data_x_ptr (float *): Location where the data from the x axis should be stored.
 * @param data_y_ptr (float *): Location where the data from the y axis should be stored.
 * @param data_z_ptr (float *): Location where the data from the z axis should be stored.
 * @retval (ret_t): Success or failure of putting the new packet onto the accel queue.
 */
ret_t LSM303DLHC_accel_putPacket(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
{
    // Get the data from the queue...
    accel_packet_t pkt;
    pkt.timestamp = HAL_GetTick();
    pkt.x = *data_x_ptr;
    pkt.y = *data_y_ptr;
    pkt.z = *data_z_ptr;
    LSM303DLHC.accel_pkts[LSM303DLHC.accel_queue.head_ind] = pkt;

    // move the head forward and handle overwrite cases
    queue_increment_head(&LSM303DLHC.accel_queue, ACCEL_QUEUE_SIZE);

    return RET_OK;
}

/*! Puts the given data onto the mag queue.
 *
 * Note: This is the ONLY function that modifies the head index of the mag queue.
 *   It also can modify the tail index if we're overwritting unread data.
 *
 * @param data_x_ptr (float *): Location where the data from the x axis should be stored.
 * @param data_y_ptr (float *): Location where the data from the y axis should be stored.
 * @param data_z_ptr (float *): Location where the data from the z axis should be stored.
 * @retval (ret_t): Success or failure of putting the new packet onto the mag queue.
 */
ret_t LSM303DLHC_mag_putPacket(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr)
{
    mag_packet_t pkt;
    pkt.timestamp = HAL_GetTick();
    pkt.x = *data_x_ptr;
    pkt.y = *data_y_ptr;
    pkt.z = *data_z_ptr;
    // Put the data on the queue...
    LSM303DLHC.mag_pkts[LSM303DLHC.mag_queue.head_ind] = pkt;

    // now move the head forward and handle overwrite cases
    queue_increment_head(&LSM303DLHC.mag_queue, MAG_QUEUE_SIZE);

    return RET_OK;
}

/*! Gets data from the accel queue.
 *
 * Note: This is function modifies the tail index if we're overwritting unread data.
 *   LSM303DLHC_accel_putPacket also can modify it, so we must check for that case in
 *   this function as LSM303DLHC_accel_getPacket gets called asynchronously.
 *
 * @param pkt_ptr (accel_packet_t *): Place to put the retreived accel packet.
 * @param peak (bool): Whether or not we should pop it off the queue or to just peak at it.
 * @retval (ret_t): Success or failure of getting the accel packet off the queue.
 */
ret_t LSM303DLHC_accel_getPacket(accel_packet_t * pkt_ptr, bool peak)
{
    uint8_t queue_ind = LSM303DLHC.accel_queue.tail_ind;
    if ( LSM303DLHC_accel_dataAvailable() ) {

        // Get the data from the queue...
        *pkt_ptr = LSM303DLHC.accel_pkts[queue_ind];

        // Check if this data is valid (could've been overwritten mid way through)
        if (queue_ind != LSM303DLHC.accel_queue.tail_ind) {
            return RET_COM_ERR;
        } else if (peak == false) {
            // the queue didn't change while we were getting it and we aren't just peaking
            queue_increment_tail(&LSM303DLHC.accel_queue, ACCEL_QUEUE_SIZE);
            return RET_OK;
        }
    } else {

        return RET_NODATA_ERR;
    }
    return RET_OK;
}

/*! Gets data from the mag queue.
 *
 * Note: This is function modifies the tail index if we're overwritting unread data.
 *   LSM303DLHC_mag_putPacket also can modify it, so we must check for that case in
 *   this function as LSM303DLHC_mag_getPacket gets called asynchronously.
 *
 * @param pkt_ptr (mag_packet_t *): Place to put the retreived mag packet.
 * @param peak (bool): Whether or not we should pop it off the queue or to just peak at it.
 * @retval (ret_t): Success or failure of getting the mag packet off the queue.
 */
ret_t LSM303DLHC_mag_getPacket(mag_packet_t * pkt_ptr, bool peak)
{
    uint8_t queue_ind = LSM303DLHC.mag_queue.tail_ind;
    if ( LSM303DLHC_mag_dataAvailable() ) {

        // Get the data from the queue...
        *pkt_ptr = LSM303DLHC.mag_pkts[queue_ind];

        // Check if this data is valid (could've been overwritten mid way through)
        if (queue_ind != LSM303DLHC.mag_queue.tail_ind) {
            return RET_COM_ERR;
        } else if (peak == false) {
            // the queue didn't change while we were getting it and we aren't just peaking
            queue_increment_tail(&LSM303DLHC.mag_queue, MAG_QUEUE_SIZE);
            return RET_OK;
        }
    } else {

        return RET_NODATA_ERR;
    }
    return RET_OK;
}

/*! Checks if there is data available on our accel queue.
 *
 * @retval (bool): True if there are available packets.
 */
bool LSM303DLHC_accel_dataAvailable(void)
{
    if (LSM303DLHC.accel_queue.unread_items == 0) {
        return false;
    } else {
        return true;
    }
}

/*! Checks if there is data available on our mag queue.
 *
 * @retval (bool): True if there are available packets.
 */
bool LSM303DLHC_mag_dataAvailable(void)
{
    if (LSM303DLHC.mag_queue.unread_items == 0) {
        return false;
    } else {
        return true;
    }
}

/*! Gets the number of packet drops in the accel queue.
 *
 * @retval (uint8_t): Number of queue overwrites.
 */
uint8_t LSM303DLHC_accel_packetOverwriteCount(void)
{
    return LSM303DLHC.accel_queue.overwrite_count;
}

/*! Gets the number of packet drops in the mag queue.
 *
 * @retval (uint8_t): Number of queue overwrites.
 */
uint8_t LSM303DLHC_mag_packetOverwriteCount(void)
{
    return LSM303DLHC.mag_queue.overwrite_count;
}

/*! Gets the number of HW related drops from the chip.
 *
 * Note: Is not 100% accurate as there is only a flag for overwrite on LSM303DLHC
 *    rather than a counter.
 *
 * @retval (uint8_t): Number of hardware overwrites that ocurred on the LSM303DLHC.
 */
uint32_t LSM303DLHC_accel_HardwareOverwriteCount(void)
{
    return LSM303DLHC.accel_hw_overwrite_count;
}

/*! Gets the number of HW related drops from the chip.
 *
 * Note: Is not 100% accurate as there is only a flag for overwrite on LSM303DLHC
 *    rather than a counter.
 *
 * @retval (uint8_t): Number of hardware overwrites that ocurred on the LSM303DLHC.
 */
uint32_t LSM303DLHC_mag_HardwareOverwriteCount(void)
{
    return LSM303DLHC.mag_hw_overwrite_count;
}
