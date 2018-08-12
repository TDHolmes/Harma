/*!
 * @file    matrixmath.c
 * @author  Tyler Holmes
 *
 * @date    28-May-2017
 * @brief   Module for doing matrix / vector math calculations.
 */
#include "matrixmath.h"
#include "common.h"
#include <stdint.h>
#include <stdio.h>

/*! Calculates the matrix multiply of the two vectors given, in that order.
 *
 * @Note: The user is responsible for allocating the proper amount of memory
 *    in all three matrices before entering this function.
 *
 * @param m0 (matrix_3x3_t): First matrix in the multiply. Order matters!
 * @param m1 (matrix_1x3_t): Second matrix in the multiply. Order matters!
 * @param m_result (matrix_1x3_t *): Pointer to the matrix we are to store the result in.
 * @return retval (ret_t): Success or failure reason of the multiply.
 */
ret_t matrix_multiply(matrix_3x3_t m0, matrix_1x3_t m1, matrix_1x3_t *m_result)
{
    // loop through the matrices and do the math!
    for (uint32_t y = 0; y < 3; y++) {
        float sum = 0;
        for (uint32_t x = 0; x < 3; x++) {
            sum += m0.matrix[x][y] * m1.matrix[0][x]; // TODO: generalize for N dimensional?
        }
        m_result->matrix[0][y] = sum;
    }
    return RET_OK;
}

/*! Calculates the cross product of the given vectors. m0 x m1 = m_result
 *
 * @Note: Currently only supports vectors of 1x3 size
 *
 * @param m0 (matrix_1x3_t): First matrix in the cross product. Order matters!
 * @param m1 (matrix_1x3_t): Second matrix in the cross product. Order matters!
 * @param m_result (matrix_1x3_t *): Pointer to the matrix we are to store the result in.
 * @return retval (ret_t): Success or failure reason of the cross product.
 */
ret_t matrix_cross(matrix_1x3_t m0, matrix_1x3_t m1, matrix_1x3_t *m_result)
{

    // now, generate the matrix to multiply m1 by with m0 to perform the cross product
    matrix_3x3_t cm;

    // Make the cross matrix...
    cm.matrix[0][0] = 0;
    cm.matrix[1][0] = -m0.matrix[0][2];
    cm.matrix[2][0] = m0.matrix[0][1];
    cm.matrix[0][1] = m0.matrix[0][2];
    cm.matrix[1][1] = 0;
    cm.matrix[2][1] = -m0.matrix[0][0];
    cm.matrix[0][2] = -m0.matrix[0][1];
    cm.matrix[1][2] = m0.matrix[0][0];
    cm.matrix[2][2] = 0;

    // Now it's a simply multiply of the cm matrix and m1
    return matrix_multiply(cm, m1, m_result);
}
