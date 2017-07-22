/*!
 * @file    matrixmath.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    28-May-2017
 * @brief   Module for doing matrix / vector math calculations.
 */
#pragma once

#include <stdint.h>
#include "common.h"

// Public data structures
typedef struct {
    float matrix[3][3];     //!< 2D array of the data in the matrix
} matrix_3x3_t;

typedef struct {
    float matrix[1][3];     //!< 2D array of the data in the matrix
} matrix_1x3_t;


// Public function definitions
ret_t matrix_multiply(matrix_3x3_t m0, matrix_1x3_t m1, matrix_1x3_t * m_result);
ret_t matrix_cross(matrix_1x3_t m0, matrix_1x3_t m1, matrix_1x3_t * m_result);
