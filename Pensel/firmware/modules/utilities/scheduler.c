/*!
 * @file    scheduler.c
 * @author  Tyler Holmes
 *
 * @date    28-April-2018
 * @brief   A rudamentary scheduler
 */

#include "scheduler.h"
#include "common.h"
#include "logging.h"
#include <stdbool.h>
#include <stdint.h>

/* TODO: My implementation of schedule_id is fundamentally flawed. We should be able to ask the
 *   scheduler if a callback is still registered. This then means we need _unique_ ids for the
 *   ID. We then need a dictionary of active schedule_id's to slots in the schedule, and implement
 *   a scheme to give unique schedule IDs. (probably just a running incrmenting base plus index)
 */

/* -- Private Function Prototypes -- */

int getSlotForID(schedule_t *schedule, schedule_id_t id);
int getIDForSlot(schedule_t *schedule, int slot);
bool slotIsAvailable(schedule_slot_t *slot);
bool slotIsValid(schedule_slot_t *slot);

/* -- Public Functions -- */

ret_t scheduler_init(schedule_t *schedule)
{
    schedule->next_unique_id = 1;
    schedule->num_slots_used = 0;
    for (uint8_t i = 0; i < NUM_SLOTS; i++) {
        schedule->slots[i].id = SCHEDULER_INVALID_ID;
        schedule->slots[i].callback = NULL;
        schedule->slots[i].callback_times_ms = 0;
        schedule->slots[i].callback_last_run_ms = 0;
    }
    return RET_OK;
}

// TODO: not sure this is interrupt preemption safe...
ret_t scheduler_add(schedule_t *schedule, int32_t callback_time_ms,
                    ret_t (*callback)(int32_t *callback_time_ms), schedule_id_t *schedule_id)
{
    bool slot_found = false;
    if (schedule->num_slots_used >= NUM_SLOTS) {
        return RET_LEN_ERR;
    }
    // TODO: disable timer interrupt here
    for (uint8_t i = 0; i < NUM_SLOTS; i++) {
        if ( slotIsAvailable(&schedule->slots[i]) ) {
            // Slot found! mark it as used!
            schedule->slots[i].id = schedule->next_unique_id;
            slot_found = true;
            *schedule_id = schedule->next_unique_id;
            schedule->slots[i].callback = callback;
            schedule->slots[i].callback_times_ms = callback_time_ms;
            schedule->num_slots_used += 1;

            /* Get the next unique ID available in this schedule. Increment
             *   the ID until we find one not in use. Normally this should only
             *   run once.
             */
            do {
                schedule->next_unique_id += 1;
            } while( getSlotForID(schedule, schedule->next_unique_id) != -1 );
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

ret_t scheduler_remove(schedule_t *schedule, schedule_id_t schedule_id_to_remove)
{
    ret_t err = RET_OK;

    // TODO: disable timer interrupt here

    // Check if the given schedule ID even exists in here
    int idex = getSlotForID(schedule, schedule_id_to_remove);
    if (idex == -1) {
        err = RET_VAL_ERR;
    } else {
        // valid schedule ID!
        schedule_slot_t *slot = &schedule->slots[idex];
        slot->callback_last_run_ms = 0;
        slot->callback_times_ms = 0;
        slot->callback = NULL;
        schedule->num_slots_used -= 1;
        slot->id = SCHEDULER_INVALID_ID;  // This marks the slot as available for use
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
        schedule_slot_t *slot = &schedule->slots[i];
        if ( slotIsValid(slot) ) {
            // check if we need to run this guy
            // set it to the period, then subtract the amount of ms since last called
            int32_t time_until_next_run = slot->callback_times_ms;
            time_until_next_run -= (current_time_ms - slot->callback_last_run_ms);
            // If zero or negative, we are due to run it
            if (time_until_next_run <= 0) {
                // TODO: check & log if we're missing the timing requirements of this callback
                // Call the function
                ret = slot->callback(&new_callback_time);
                if (ret != RET_OK) {
                    LOG_MSG_FMT(kLogLevelWarning, "callback %lux returned %i", (uint32_t)slot->callback, ret);
                }

                if (new_callback_time == SCHEDULER_FINISHED) {
                    scheduler_remove(schedule, getIDForSlot(schedule, i));
                } else {
                    slot->callback_last_run_ms = current_time_ms;
                    slot->callback_times_ms = new_callback_time;
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

bool scheduler_IDInSchedule(schedule_t *schedule, schedule_id_t id) {
    return getSlotForID(schedule, id) != -1;
}

/* -- Private Functions -- */

/*! Returns the active slot for a given schedule ID. Returns -1 if that schedule ID
 *    is not in the given schedule.
 *
 * @param schedule  the schedule in question
 */
int getSlotForID(schedule_t *schedule, schedule_id_t id) {
    int slot = -1;
    if (id != SCHEDULER_INVALID_ID) {
        for (int i=0; i<NUM_SLOTS; i++) {
            if (schedule->slots[i].id == id) {
                slot = i;
                break;
            }
        }
    }
    return slot;
}

/*! Returns the schedule ID for a given slot index. The returned schedule ID might be
 *    invalid if the slot given doesn't have a valid, active callback.
 *
 * @param schedule  the schedule in question
 */
int getIDForSlot(schedule_t *schedule, int slot) {
    if (slot < 0 || slot >= NUM_SLOTS) {
        return SCHEDULER_INVALID_ID;
    }
    return schedule->slots[slot].id;
}

/*! helper function to check if a given slot is de-initialized and available for use
 */
bool slotIsAvailable(schedule_slot_t *slot) {
    return slot->id == SCHEDULER_INVALID_ID;
}

/*! helper function to check if a given slot is properly initialized and callable.
 */
bool slotIsValid(schedule_slot_t *slot) {
    return slot->id != SCHEDULER_INVALID_ID && slot->callback != NULL;
}