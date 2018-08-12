/*!
 * @file    orientation.h
 * @author  Tyler Holmes
 *
 * @date    28-May-2017
 * @brief   Module for calculating Pensel's orientation in space.
 *
 * This is done by using orientations provided by Accel's gravity vector and Mag's north vector.
 */
#pragma once

#include "common.h"
#include "modules/LSM303DLHC/LSM303DLHC.h"
#include "quanternions.h"
#include <stdint.h>

ret_t orient_init(void);
void orient_calcPenselOrientation(void);
void orient_calcMagOrientation(mag_norm_t pkt);
void orient_calcAccelOrientation(accel_norm_t pkt);
cartesian_vect_t orient_getPenselOrientation(void);
cartesian_vect_t orient_getMagOrientation(void);
cartesian_vect_t orient_getAccelOrientation(void);

// Reports for getting the orientations
ret_t rpt_orient_getPenselOrientation(uint8_t *UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                      uint8_t *out_p, uint8_t *out_len_ptr);
ret_t rpt_orient_getMagOrientation(uint8_t *UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                   uint8_t *out_p, uint8_t *out_len_ptr);
ret_t rpt_orient_getAccelOrientation(uint8_t *UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                     uint8_t *out_p, uint8_t *out_len_ptr);
