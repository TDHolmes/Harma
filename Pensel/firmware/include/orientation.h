/*!
 * @file    orientation.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's orientation in space.
 *
 * This is done by using orientations provided by Accel's gravity vector and Mag's north vector.
 */
#ifndef _ORIENTATION_H_
#define _ORIENTATION_H_

#include <stdint.h>
#include "common.h"


ret_t orient_init(void);

#endif /* _ORIENTATION_H_ */
