/*!
 * @file    quanternions.c
 * @author  Tyler Holmes
 *
 * @date    28-May-2017
 * @brief   Module for doing quanternion-y things.
 */
#include <stdint.h>
#include <math.h>  // TODO: Don't use this library. Need single percision sine or LUT
#include "quanternions.h"


quanternion_vect_t quanternion_create(cartesian_vect_t eigen_axis, float rotation_angle)
{
    quanternion_vect_t new_vect;

    // TODO: Don't use this horrible library function that is double percision
    new_vect.one = eigen_axis.x * (float)sin(rotation_angle * 0.5f);
    new_vect.two = eigen_axis.y * (float)sin(rotation_angle * 0.5f);
    new_vect.three = eigen_axis.z * (float)sin(rotation_angle * 0.5f);
    new_vect.four = (float)cos(rotation_angle * 0.5f);

    return new_vect;
}


// conversion methods
cartesian_vect_t quanternion_toCartesian(quanternion_vect_t vector)
{
    cartesian_vect_t new_vect;

    return new_vect;
}


quanternion_vect_t quanternion_fromCartesian(cartesian_vect_t vector)
{
    quanternion_vect_t new_vect;

    return new_vect;
}
