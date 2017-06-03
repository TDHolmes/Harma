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
#include "accel_coefficients.h"
#include "mag_coefficients.h"


#define X_IND (0) //!< When dealing with a 3 element array representing cartesian coordinates, this is the X index
#define Y_IND (1) //!< When dealing with a 3 element array representing cartesian coordinates, this is the Y index
#define Z_IND (2) //!< When dealing with a 3 element array representing cartesian coordinates, this is the Z index


typedef struct {
    FIR_admin_t * FIR_accelGrav_ptr[3];  //!< an FIR filter for all 3 axes of accel for gravity detection
    FIR_admin_t * FIR_magNorth_ptr[3];   //!< an FIR filter for all 3 axes of mag for north detection
    quanternion_vector_t north_vector;   //!< Detected north vector in quanternion form
    quanternion_vector_t gravity_vector; //!< Detected gravity vector in quanternion form
    quanternion_vector_t pensel_vector;  //!< Calculated pensel orientation in quanternion form
} orientation_admin_t;


orientation_admin_t orient;


ret_t orient_init(void)
{
    ret_t retval;
    // Initialize all 3 vectors for accel gravitation filtering
    retval = FIR_init(orient.FIR_accelGrav_ptr[X_IND], FIR_ACCEL_GRAVITY_ORDER,
                      accel_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(orient.FIR_accelGrav_ptr[Y_IND], FIR_ACCEL_GRAVITY_ORDER,
                      accel_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(orient.FIR_accelGrav_ptr[Z_IND], FIR_ACCEL_GRAVITY_ORDER,
                      accel_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    // Initialize all 3 vectors for mag north filtering
    retval = FIR_init(orient.FIR_magNorth_ptr[X_IND], FIR_MAG_NORTH_ORDER,
                      mag_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(orient.FIR_magNorth_ptr[Y_IND], FIR_MAG_NORTH_ORDER,
                      mag_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    retval = FIR_init(orient.FIR_magNorth_ptr[Z_IND], FIR_MAG_NORTH_ORDER,
                      mag_coefficients_LPF);
    if (retval != RET_OK) { return retval; }

    return RET_OK;
}


void orient_run(void)
{
    // update magnetic north & gravity vectors, if new data available
    // Calculated pensel orientation if new mag / gravity packets
}


quanternion_vector_t orient_calcPenselOrientation(void)
{
    quanternion_vector_t pensel_orientation;
    // Calculate pensel orientation from orient.north_vector and orient.gravity_vector!

    return pensel_orientation;
}


void orient_calcMagOrientation(mag_packet_t pkt)
{
    // Update orient.north_vector!
    // TODO: low pass filter on raw mag data to get good north readings
}


void orient_calcAccelOrientation(accel_packet_t pkt)
{
    // Update orient.gravity_vector!
    // TODO: low pass filter on raw accel data to get good gravity readings
}
