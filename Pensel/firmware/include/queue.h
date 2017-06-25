/*!
 * @file    queue.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Simple circular buffer handler.
 *
 */
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>


//! The structure to track all pertinant variables for the circular buffer.
typedef struct {
    volatile uint8_t head_ind;  //!< Tracks the front of the buffer (where new info gets pushed)
    volatile uint8_t tail_ind;  //!< Tracks the back of the buffer (where data is read from)
    volatile uint8_t unread_items;    //!< Number of un-popped items
    volatile uint32_t overwrite_count; //!< Number of items that were lost to overwrite
} queue_t;


void queue_increment_tail(volatile queue_t * queue_admin_ptr, const uint8_t queue_size);
void queue_increment_head(volatile queue_t * queue_admin_ptr, const uint8_t queue_size);

#endif /* _QUEUE_H_ */
