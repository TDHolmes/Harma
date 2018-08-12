/*!
 * @file    newqueue.h
 * @author  Tyler Holmes
 *
 * @date    20-May-2017
 * @brief   General purpose circular buffer handler that uses malloc.
 *
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "common.h"


//! Keeps track of all variables needed to track the circular buffer.
typedef struct {
    volatile uint32_t head_ind;  //!< Tracks the front of the buffer (where new info gets pushed)
    volatile uint32_t tail_ind;  //!< Tracks the back of the buffer (where data is read from)
    volatile uint32_t unread_items;    //!< Number of un-popped items
    volatile uint32_t overwrite_count; //!< Number of items that were lost to overwrite
    uint32_t num_items;    //!< The number of items this queue can hold
    uint32_t item_size;    //!< The size (in bytes) of the items to be stored (e.g. 4 for uint32_t)
    uint32_t buffer_size;  //!< Length of the buffer that was instanciated at runtime
    void * buff_ptr;       //!< Pointer to the allocated memory
} newqueue_t;


typedef enum {
    eNoPeak,
    ePeak
} peak_t;


ret_t newqueue_init(volatile newqueue_t * newqueue, uint32_t num_elements, uint32_t item_size);
ret_t newqueue_deinit(volatile newqueue_t * newqueue);
ret_t newqueue_pop(volatile newqueue_t * queue, void * data_ptr, uint32_t num_items, peak_t peak);
ret_t newqueue_push(volatile newqueue_t * queue, void * data_ptr, uint32_t num_items);
