/*!
 * @file    scheduler.c
 * @author  Tyler Holmes
 *
 * @date    28-April-2018
 * @brief   A rudamentary scheduler
 */

#include "scheduler.h"
#include "common.h"
#include <stdbool.h>
#include <stdint.h>

// TODO remove - debug
// #include "peripherals/UART/UART.h"

ret_t scheduler_init(schedule_t *schedule)
{
    schedule->num_slots_used = 0;
    for (uint8_t i = 0; i < NUM_SLOTS; i++) {
        schedule->callbacks[i] = NULL;
        schedule->callback_times_ms[i] = 0;
        schedule->callback_last_run_ms[i] = 0;
    }
    return RET_OK;
}

ret_t scheduler_add(schedule_t *schedule, int32_t callback_time_ms,
                    ret_t (*callback)(int32_t *callback_time_ms), uint8_t *schedule_id)
{
    bool slot_found = false;
    if (schedule->num_slots_used >= NUM_SLOTS) {
        return RET_LEN_ERR;
    }
    // TODO: disable timer interrupt here
    for (uint8_t i = 0; i < NUM_SLOTS; i++) {
        if (schedule->callbacks[i] == NULL) {
            schedule->callbacks[i] = callback;
            schedule->callback_times_ms[i] = callback_time_ms;
            slot_found = true;
            schedule->num_slots_used += 1;
            *schedule_id = i;
            break;
        }
    }
    // TODO: re-enable timer interrupt here

    if (slot_found == true) {
        return RET_OK;
    } else {
        return RET_NOMEM_ERR;
    }
}

ret_t scheduler_remove(schedule_t *schedule, uint8_t schedule_id_to_remove)
{
    ret_t err = RET_OK;

    // TODO: disable timer interrupt here
    // Check if the given schedule ID even exists in here
    if (schedule->callbacks[schedule_id_to_remove] != NULL) {
        schedule->callbacks[schedule_id_to_remove] = NULL;
        schedule->callback_times_ms[schedule_id_to_remove] = 0;
        schedule->num_slots_used -= 1;
    } else {
        err = RET_VAL_ERR;
    }
    // TODO: reenable timer interrupt here

    return err;
}

int32_t scheduler_run(schedule_t *schedule, uint32_t current_time_ms)
{
    // uint8_t num_schedules_to_run = 0;
    // int16_t this_schedule[NUM_SLOTS];
    // TODO: smartly call the "most starved" callbacks first?
    int32_t new_callback_time = 0;
    int32_t next_cb_time_ms = INT32_MAX; // default value is max possible
    ret_t ret;

    // TODO: not sure if I need to disable timer interrupts here...
    for (uint8_t i = 0; i < NUM_SLOTS; i++) {
        if (schedule->callbacks[i] != NULL) {
            // check if we need to run this guy
            // set it to the period, then subtract the amount of ms since last called
            int32_t time_until_next_run = schedule->callback_times_ms[i];
            time_until_next_run -= (current_time_ms - schedule->callback_last_run_ms[i]);
            // If zero or negative, we are due to run it
            if (time_until_next_run <= 0) {
                // TODO: check & log if we're missing the timing requirements of this callback
                // Call the function
                ret = schedule->callbacks[i](&new_callback_time);
                // TODO: log if the callback returns non RET_OK
                if (new_callback_time == SCHEDULER_FINISHED) {
                    scheduler_remove(schedule, i);
                } else {
                    schedule->callback_last_run_ms[i] = current_time_ms;
                    schedule->callback_times_ms[i] = new_callback_time;
                    time_until_next_run = new_callback_time;
                }
            }

            // Check if this will be the next callback we need to run next time
            if (time_until_next_run < next_cb_time_ms) {
                next_cb_time_ms = time_until_next_run;
            }
        }
    }

    return next_cb_time_ms;
}
