/*
 *
 *
 */
 #ifndef _LSM303DLHC_H_
 #define _LSM303DLHC_H_

#include <stdint.h>
#include <stdbool.h>
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
    kAccelODR_PowerDown,
    kAccelODR_1Hz,
    kAccelODR_10_Hz,
    kAccelODR_25_Hz,
    kAccelODR_50_Hz,
    kAccelODR_100_Hz,
    kAccelODR_200_Hz,
    kAccelODR_400_Hz,
    kAccelODR_1620_Hz,
    kAccelODR_N_1344_LPM_5376_Hz
} accel_ODR_t;


typedef enum {
    kMagODR_0_75_Hz,
    kMagODR_1_5_Hz,
    kMagODR_3_Hz,
    kMagODR_7_5_Hz,
    kMagODR_15_Hz,
    kMagODR_30_Hz,
    kMagODR_75_Hz,
    kMagODR_220_Hz
} mag_ODR_t;


typedef enum {
    kFIFO_MODE_Bypass,
    kFIFO_MODE_FIFO,
    kFIFO_MODE_Stream,
    kFIFO_MODE_Trigger
} FIFO_mode_t;


typedef struct {
    uint32_t timestamp;  //!< Timestamp when the packet was received
    int32_t x;           //!< Accel X value
    int32_t y;           //!< Accel Y value
    int32_t z;           //!< Accel Z value
} accel_packet_t;


typedef struct {
    uint32_t timestamp;  //!< Timestamp when the packet was received
    float x;           //!< Mag X value
    float y;           //!< Mag Y value
    float z;           //!< Mag Z value
} mag_packet_t;


ret_t LSM303DLHC_init(accel_ODR_t accel_datarate, accel_sensitivity_t accel_sensitivity,
                      mag_ODR_t mag_datarate, mag_sensitivity_t mag_sensitivity);

// functions to get packets and check for data!
bool LSM303DLHC_accel_dataAvailable(void);
bool LSM303DLHC_mag_dataAvailable(void);
ret_t LSM303DLHC_accel_getPacket(accel_packet_t * pkt_ptr, bool peak);
ret_t LSM303DLHC_mag_getPacket(mag_packet_t * pkt_ptr, bool peak);

// direct call to get the temperature
ret_t LSM303DLHC_temp_getData(int16_t * temp_val_ptr);

// functions to be called by the hardware pin interrupt handler
void LSM303DLHC_drdy_handler(void);
// void LSM303DLHC_int_handler(void);


#endif /* _LSM303DLHC_H_ */
