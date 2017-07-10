/*!
 * @file    quanternions.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    28-May-2017
 * @brief   Module for doing quanternion-y things.
 */
#include <stdint.h>
#include <math.h>
#include "quanternions.h"


quanternion_vect_t quanternion_create(cartesian_vect_t eigen_axis, float rotation_angle)
{
    quanternion_vect_t new_vect;

    // TODO: Don't use this horrible library function
    new_vect.one = eigen_axis.x * (float)sin(rotation_angle * 0.5f);
    new_vect.two = eigen_axis.y * (float)sin(rotation_angle * 0.5f);
    new_vect.three = eigen_axis.z * (float)sin(rotation_angle * 0.5f);
    new_vect.four = (float)cos(rotation_angle * 0.5f);

    return new_vect;
}


dcs_t quanternion_calcDCS(quanternion_vect_t vector)
{
    dcs_t dcs;

    dcs.vector[0][0] = 1 - 2 * (vector.two * vector.two + vector.three * vector.three);
    dcs.vector[1][1] = 1 - 2 * (vector.one * vector.one + vector.three * vector.three);
    dcs.vector[2][2] = 1 - 2 * (vector.one * vector.one + vector.two * vector.two);

    dcs.vector[0][1] = 2 * (vector.one * vector.two - vector.three * vector.four);
    dcs.vector[1][0] = 2 * (vector.one * vector.two + vector.three * vector.four);

    dcs.vector[0][2] = 2 * (vector.one * vector.three + vector.two * vector.four);
    dcs.vector[2][0] = 2 * (vector.one * vector.three - vector.two * vector.four);

    dcs.vector[2][1] = 2 * (vector.two * vector.three + vector.one * vector.four);
    dcs.vector[1][2] = 2 * (vector.two * vector.three + vector.one * vector.four);

    return dcs;
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
