#pragma once

#include "cal.h"

#define DEFAULT_CAL_CHECKSUM (0)

// TODO: fill in default cal values
pensel_cal_t default_cal = {
    .header = PENSEL_CAL_HEADER,
    .version = PENSEL_CAL_VERSION,
    .accel_offsets = {0, 0, 0},
    .accel_gains = {1, 1, 1},
    .mag_offsets = {0, 0, 0},
    .mag_gains = {1, 1, 1},
    .padding = {0},
    .checksum = DEFAULT_CAL_CHECKSUM,
};
