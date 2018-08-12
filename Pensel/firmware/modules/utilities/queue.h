/*!
 * @file    queue.h
 * @author  Tyler Holmes
 *
 * @date    20-May-2017
 * @brief   Simple circular buffer handler.
 */
#pragma once

#include <stdint.h>


//! The structure to track all pertinant variables for the circular buffer.
typedef struct {
    volatile uint8_t head_ind;  //!< Tracks the front of the buffer (where new info gets pushed)
    volatile uint8_t tail_ind;  //!< Tracks the back of the buffer (where data is read from)
    volatile uint8_t unread_items;    //!< Number of un-popped items
    volatile uint32_t overwrite_count; //!< Number of items that were lost to overwrite
} queue_t;


void queue_init(volatile queue_t * queue_admin_ptr);
void queue_increment_tail(volatile queue_t * queue_admin_ptr, const uint8_t queue_size);
void queue_increment_head(volatile queue_t * queue_admin_ptr, const uint8_t queue_size);
