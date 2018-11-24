/*!
 * @file    scheduler.h
 * @author  Tyler Holmes
 *
 * @date    28-April-2018
 * @brief   A rudamentary scheduler
 */

#pragma once

#include "common.h"
#include <stdint.h>
#include <stdbool.h>


#define NUM_SLOTS (16)
#define SCHEDULER_FINISHED (-1)   //!< What the callback should return so we don't call it again
#define SCHEDULER_INVALID_ID  ((schedule_id_t)-1)

//! a unique ID given to a callback in a schedule. Will never be 0.
typedef uint32_t schedule_id_t;

/*! The struct defining a slot within a schedule. The implementation details in this
 *    struct should NOT be relied upon or modified!
 */
typedef struct {
    volatile schedule_id_t id;      //!< The unique (WRT the given schedule) ID for this slot.
    int32_t callback_times_ms;      //!< The time this slot should next be called
    int32_t callback_last_run_ms;   //!< The last time this callback was called
    //! The callback to be called for this slot in the schedule
    ret_t (* volatile callback)(int32_t *callback_time_ms);
} schedule_slot_t;

/*! The struct defining an active schedule. The implementation details in this struct
 *    should not be relied upon or modified!
 */
struct _schedule_t {
    volatile uint8_t num_slots_used;         //!< a quick check if the schedule is full or not
    volatile schedule_id_t next_unique_id;   //!< The next available schedule ID to be used
    schedule_slot_t slots[NUM_SLOTS];        //!< list of slots in the schedule
};

typedef struct _schedule_t schedule_t;

/*!
 *
 * @param schedule  the schedule in question
 */
ret_t scheduler_init(schedule_t *schedule);

/*!
 *
 * @param schedule  the schedule in question
 *
 * @note This function is thread-safe and can be called in interrupt context.
 */
ret_t scheduler_add(schedule_t *schedule,
                    int32_t callback_time_ms,
                    ret_t (*callback)(int32_t *callback_time_ms),
                    schedule_id_t *schedule_id);

/*!
 *
 * @param schedule  the schedule in question
 *
 * @warn This function cannot be called in interrupt context!
 */
ret_t scheduler_remove(schedule_t *schedule,
                       schedule_id_t schedule_id_to_remove);

/*!
 *
 * @param schedule  the schedule in question
 */
int32_t scheduler_run(schedule_t *schedule,
                      uint32_t current_time_ms);

/*!
 *
 * @param schedule  the schedule in question
 */
bool scheduler_IDInSchedule(schedule_t *schedule,
                            schedule_id_t id);
