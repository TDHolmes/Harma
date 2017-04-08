/*
 *
 *
 */
 #ifndef _LSM303DLHC_H_
 #define _LSM303DLHC_H_

#include <stdint.h>
#include "common.h"


typedef enum {
    kTwoG,
    kFourG,
    kEightG,
    kSixteenG,
} accel_fullscale_t;

typedef enum {
    kOne_Three_gauss = 1,
    kOne_Nine_gauss,
    kTwo_Five_gauss,
    kFour_Zero_gauss,
    kFour_Seven_gauss,
    kFive_Six_gauss,
    kEight_One_gauss,
} mag_fullscale_t;

typedef enum {
    kOne_mg_per_LSB,
    kTwo_mg_per_LSB,
    kFour_mg_per_LSB,
    kTwelve_mg_per_LSB
} accel_sensitivity_t;

typedef enum {
    kXY_1100_Z_980_LSB_per_g = 1,
    kXY_855_Z_760_LSB_per_g,
    kXY_670_Z_600_LSB_per_g,
    kXY_450_Z_400_LSB_per_g,
    kXY_400_Z_355_LSB_per_g,
    kXY_330_Z_295_LSB_per_g,
    kXY_230_Z_205_LSB_per_g
} mag_sensitivity_t;

typedef enum {
    kODR_PowerDown,
    kODR_1Hz,
    kODR_10_Hz,
    kODR_25_Hz,
    kODR_50_Hz,
    kODR_100_Hz,
    kODR_200_Hz,
    kODR_400_Hz,
    kODR_1620_Hz,
    kODR_N_1344_LPM_5376_Hz
} accel_ODR_t;


typedef enum {
    kFIFO_MODE_Bypass,
    kFIFO_MODE_FIFO,
    kFIFO_MODE_Stream,
    kFIFO_MODE_Trigger
} FIFO_mode_t;


ret_t LSM303DLHC_init(void);
ret_t LSM303DLHC_accel_getdata(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);
ret_t LSM303DLHC_mag_getdata(float * data_x_ptr, float * data_y_ptr, float * data_z_ptr);


#endif /* _LSM303DLHC_H_ */
