/*!
 * @file    scheduler.h
 * @author  Tyler Holmes
 *
 * @date    28-April-2018
 * @brief   A rudamentary scheduler
 */

#pragma once

#include <stdint.h>
#include "common.h"

#define NUM_SLOTS (16)
#define SCHEDULER_FINISHED  (-1)

typedef struct {
    uint8_t num_slots_used;
    ret_t (*callbacks[NUM_SLOTS])(int32_t * callback_time_ms);
    int32_t callback_times_ms[NUM_SLOTS];
    int32_t callback_last_run_ms[NUM_SLOTS];
} schedule_t;


ret_t scheduler_init(schedule_t * schedule);
ret_t scheduler_add(schedule_t * schedule,
                    int32_t callback_time_ms,
                    ret_t (*callback)(int32_t * callback_time_ms),
                    uint8_t * schedule_id);
ret_t scheduler_remove(schedule_t * schedule, uint8_t schedule_id_to_remove);

int32_t scheduler_run(schedule_t * schedule, uint32_t current_time_ms);
