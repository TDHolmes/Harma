
// CTRL_REG6_XL
typedef enum {
    k2g_fullscale,
    k16g_fullscale,
    k4g_fullscale,
    k8g_fullscale,
} accel_fullscale_t;

// CTRL_REG2_M
typedef enum {
    k4gauss_fullscale,
    k8gauss_fullscale,
    k12gauss_fullscale,
    k16gauss_fullscale
} mag_fullscale_t;

// CTRL_REG1_G
typedef enum {
    k245DPS_fullscale = 0,
    k500DPS_fullscale = 1,
    k2000DPS_fullscale = 3,
} gyro_fullscale_t;


// CTRL_REG1_G
typedef enum {
    kGyroODR_OFF,
    kGyroODR_14_9,
    kGyroODR_59_5,
    kGyroODR_119,
    kGyroODR_238,
    kGyroODR_476,
    kGyroODR_952
} gyro_ODR_t;
