/*!
 * @file    queue.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Simple circular buffer handler.
 *
 */
#include <stdint.h>
#include "queue.h"


void queue_increment_tail(volatile queue_t * queue_admin_ptr, const uint8_t queue_size)
{
    queue_admin_ptr->unread_items -= 1;

    if (queue_admin_ptr->tail_ind < queue_size - 1) {
        queue_admin_ptr->tail_ind += 1;
    } else {
        queue_admin_ptr->tail_ind = 0;
    }
}


void queue_increment_head(volatile queue_t * queue_admin_ptr, const uint8_t queue_size)
{
    if (queue_admin_ptr->head_ind < queue_size - 1) {
        queue_admin_ptr->head_ind += 1;
    } else {
        queue_admin_ptr->head_ind = 0;
    }

    // check if we're overwriting data...
    if (queue_admin_ptr->unread_items < queue_size) {
        queue_admin_ptr->unread_items += 1;
    } else {
        // Oh no! we don't have any space. packet gets dropped :'(
        queue_admin_ptr->overwrite_count += 1;
        // move tail ahead too since we've overwritten data
        if (queue_admin_ptr->tail_ind < queue_size - 1) {
            queue_admin_ptr->tail_ind += 1;
        } else {
            queue_admin_ptr->tail_ind = 0;
        }
    }
}
