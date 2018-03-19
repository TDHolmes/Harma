/*!
 * @file    quanternions.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    28-May-2017
 * @brief   Module for doing quanternion-y things.
 */
#pragma once

#include <stdint.h>
#include "matrixmath.h"


typedef union {
    float vector[3];
    struct {
        float x;
        float y;
        float z;
    };
} cartesian_vect_t;


typedef union {
    float vector[4];
    struct {
        float one;
        float two;
        float three;
        float four;
    };
    //! Rotation about the eigenAxis
    float rotation;
} quanternion_vect_t;


quanternion_vect_t quanternion_create(cartesian_vect_t eigen_axis, float rotation_angle);
matrix_3x3_t quanternion_calcDCS(quanternion_vect_t vector);

// conversion methods
cartesian_vect_t quanternion_toCartesian(quanternion_vect_t vector);
quanternion_vect_t quanternion_fromCartesian(cartesian_vect_t vector);
