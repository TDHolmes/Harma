/*!
 * @file    orientation.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's orientation in space as well as gravity and north vectors.
 *
 * This is done by using orientations provided by Accel's gravity vector and Mag's north vector.
 */
#include <stdint.h>

#include "LSM303DLHC.h"
#include "quanternions.h"
#include "FIR.h"

// FIR coefficients
#include "FIR_coefficients.h"


#define X_IND (0) //!< When dealing with a 3 element array for cartesian coords, this is the X index
#define Y_IND (1) //!< When dealing with a 3 element array for cartesian coords, this is the Y index
#define Z_IND (2) //!< When dealing with a 3 element array for cartesian coords, this is the Z index


typedef struct {
    FIR_admin_t FIR_accelGrav_ptr[3];    //!< an FIR filter for all 3 axes of accel for gravity detection
    FIR_admin_t FIR_magNorth_ptr[3];     //!< an FIR filter for all 3 axes of mag for north detection
    quanternion_vector_t north_vector;   //!< Detected north vector in quanternion form
    quanternion_vector_t gravity_vector; //!< Detected gravity vector in quanternion form
    quanternion_vector_t pensel_vector;  //!< Calculated pensel orientation in quanternion form
} orientation_admin_t;


static orientation_admin_t orient;


ret_t orient_init(void)
{
    ret_t retval;
    // Initialize all 3 vectors for accel gravitation filtering
    retval = FIR_init(&orient.FIR_accelGrav_ptr[X_IND], FIR_ACCEL_GRAVITY_ORDER,
                      accel_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(&orient.FIR_accelGrav_ptr[Y_IND], FIR_ACCEL_GRAVITY_ORDER,
                      accel_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(&orient.FIR_accelGrav_ptr[Z_IND], FIR_ACCEL_GRAVITY_ORDER,
                      accel_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    // Initialize all 3 vectors for mag north filtering
    retval = FIR_init(&orient.FIR_magNorth_ptr[X_IND], FIR_MAG_NORTH_ORDER,
                      mag_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(&orient.FIR_magNorth_ptr[Y_IND], FIR_MAG_NORTH_ORDER,
                      mag_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(&orient.FIR_magNorth_ptr[Z_IND], FIR_MAG_NORTH_ORDER,
                      mag_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    return RET_OK;
}


void orient_calcPenselOrientation(void)
{
    // Calculate pensel orientation from orient.north_vector and orient.gravity_vector!
    // TODO: update pensel's orientation in the admin struct
}


/*! Takes in a new magnetometer packet and updates the magnetic north orientation
 *  calculation.
 *
 * @param pkt (mag_packet_t): New magnetometer packet from sensor.
 */
void orient_calcMagOrientation(mag_packet_t pkt)
{
    // Update orient.north_vector!
    // TODO: low pass filter on raw mag data to get good north readings
}

/*! Takes in a new accelerometer packet and updates the gravity vector orientation
 *  calculation.
 *
 * @param pkt (accel_packet_t): New accelerometer packet from sensor.
 */
void orient_calcAccelOrientation(accel_packet_t pkt)
{
    // Update orient.gravity_vector!
    // TODO: low pass filter on raw accel data to get good gravity readings
}

/*! Returns the currently computed pensel orientation (quanternion_vector_t)
 *
 * @return pensel_vector: The current pensel vector.
 */
quanternion_vector_t orient_getPenselOrientation(void)
{
    return orient.pensel_vector;
}

/*! Returns the currently computed magnetic north orientation (quanternion_vector_t)
 *
 * @return pensel_vector: The current north vector.
 */
quanternion_vector_t orient_getMagOrientation(void)
{
    return orient.north_vector;
}

/*! Returns the currently computed gravity orientation (quanternion_vector_t)
 *
 * @return pensel_vector: The current gravity vector.
 */
quanternion_vector_t orient_getAccelOrientation(void)
{
    return orient.gravity_vector;
}
