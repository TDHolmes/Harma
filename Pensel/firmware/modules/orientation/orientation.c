/*!
 * @file    orientation.c
 * @author  Tyler Holmes
 *
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's orientation in space as well as gravity and north
 * vectors.
 *
 * This is done by using orientations provided by Accel's gravity vector and Mag's north vector.
 */
#include "FIR_coefficients.h"
#include "modules/utilities/FIR.h"
#include "quanternions.h"
#include <stdint.h>

#define X_IND (0) //!< For 3 element arrays in cartesian cords, this is X index
#define Y_IND (1) //!< For 3 element arrays in cartesian cords, this is Y index
#define Z_IND (2) //!< For 3 element arrays in cartesian cords, this is Z index

typedef struct {
    FIR_admin_t FIR_accelGrav[3];  //!< an FIR filter for all 3 axes of accel for gravity detection
    FIR_admin_t FIR_magNorth[3];   //!< an FIR filter for all 3 axes of mag for north detection
    cartesian_vect_t north_vector; //!< Detected north vector in quanternion form
    cartesian_vect_t gravity_vector; //!< Detected gravity vector in quanternion form
    cartesian_vect_t pensel_vector;  //!< Calculated pensel orientation in quanternion form
} orientation_admin_t;

static orientation_admin_t orient;

ret_t orient_init(void)
{
    ret_t retval;
    // Initialize all 3 vectors for accel gravitation filtering
    retval =
        FIR_init(&orient.FIR_accelGrav[X_IND], FIR_ACCEL_GRAVITY_ORDER, accel_coefficients_LPF);
    if (retval != RET_OK) {
        return retval;
    }

    retval =
        FIR_init(&orient.FIR_accelGrav[Y_IND], FIR_ACCEL_GRAVITY_ORDER, accel_coefficients_LPF);
    if (retval != RET_OK) {
        return retval;
    }

    retval =
        FIR_init(&orient.FIR_accelGrav[Z_IND], FIR_ACCEL_GRAVITY_ORDER, accel_coefficients_LPF);
    if (retval != RET_OK) {
        return retval;
    }

    // Initialize all 3 vectors for mag north filtering
    retval = FIR_init(&orient.FIR_magNorth[X_IND], FIR_MAG_NORTH_ORDER, mag_coefficients_LPF);
    if (retval != RET_OK) {
        return retval;
    }

    retval = FIR_init(&orient.FIR_magNorth[Y_IND], FIR_MAG_NORTH_ORDER, mag_coefficients_LPF);
    if (retval != RET_OK) {
        return retval;
    }

    retval = FIR_init(&orient.FIR_magNorth[Z_IND], FIR_MAG_NORTH_ORDER, mag_coefficients_LPF);
    if (retval != RET_OK) {
        return retval;
    }

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
 * @param pkt (mag_norm_t): New magnetometer packet from sensor.
 */
void orient_calcMagOrientation(mag_norm_t pkt)
{
    float x, y, z;
    // Run the new data through the filters
    x = FIR_run(&orient.FIR_magNorth[X_IND], pkt.x);
    y = FIR_run(&orient.FIR_magNorth[Y_IND], pkt.y);
    z = FIR_run(&orient.FIR_magNorth[Z_IND], pkt.z);

    // Update orient.north_vector
    orient.north_vector.x = x;
    orient.north_vector.y = y;
    orient.north_vector.z = z;
}

/*! Takes in a new accelerometer packet and updates the gravity vector orientation
 *  calculation.
 *
 * @param pkt (accel_norm_t): New accelerometer packet from sensor.
 */
void orient_calcAccelOrientation(accel_norm_t pkt)
{
    float x, y, z;
    // Run the new data through the filters
    x = FIR_run(&orient.FIR_accelGrav[X_IND], pkt.x);
    y = FIR_run(&orient.FIR_accelGrav[Y_IND], pkt.y);
    z = FIR_run(&orient.FIR_accelGrav[Z_IND], pkt.z);

    // Update orient.gravity_vector
    orient.gravity_vector.x = x;
    orient.gravity_vector.y = y;
    orient.gravity_vector.z = z;
}

/*! Returns the currently computed pensel orientation (cartesian_vect_t)
 *
 * @return pensel_vector: The current pensel vector.
 */
cartesian_vect_t orient_getPenselOrientation(void) { return orient.pensel_vector; }

/*! Returns the currently computed magnetic north orientation (cartesian_vect_t)
 *
 * @return pensel_vector: The current north vector.
 */
cartesian_vect_t orient_getMagOrientation(void) { return orient.north_vector; }

/*! Returns the currently computed gravity orientation (cartesian_vect_t)
 *
 * @return pensel_vector: The current gravity vector.
 */
cartesian_vect_t orient_getAccelOrientation(void) { return orient.gravity_vector; }

/* ------------------------ REPORTS ---------------------- */

/*! Report 0x28 returns the pensel's orientation vector
 */
ret_t rpt_orient_getPenselOrientation(uint8_t *UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                      uint8_t *out_p, uint8_t *out_len_ptr)
{
    *(cartesian_vect_t *)out_p = orient_getPenselOrientation();
    *out_len_ptr = sizeof(cartesian_vect_t);
    return RET_OK;
}

/*! Report 0x29 returns the magnetic north orientation vector
 */
ret_t rpt_orient_getMagOrientation(uint8_t *UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                   uint8_t *out_p, uint8_t *out_len_ptr)
{
    *(cartesian_vect_t *)out_p = orient_getMagOrientation();
    *out_len_ptr = sizeof(cartesian_vect_t);
    return RET_OK;
}

/*! Report 0x2A returns the gravity orientation vector
 */
ret_t rpt_orient_getAccelOrientation(uint8_t *UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                     uint8_t *out_p, uint8_t *out_len_ptr)
{
    *(cartesian_vect_t *)out_p = orient_getAccelOrientation();
    *out_len_ptr = sizeof(cartesian_vect_t);
    return RET_OK;
}
