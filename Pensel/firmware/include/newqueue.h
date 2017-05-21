#ifndef _NEWQUEUE_H_
#define _NEWQUEUE_H_
/*!
 * @file    newqueue.h
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   General purpose circular buffer handler that uses malloc.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "common.h"


//! Keeps track of all variables needed to track the circular buffer.
typedef struct {
    volatile uint8_t head_ind;  //!< Tracks the front of the buffer (where new info gets pushed)
    volatile uint8_t tail_ind;  //!< Tracks the back of the buffer (where data is read from)
    volatile uint8_t unread_items;    //!< Number of un-popped items
    volatile uint8_t overwrite_count; //!< Number of items that were lost to overwrite
    uint8_t item_size;    //!< The size (in bytes) of the items to be stored (e.g. 4 for uint32_t)
    uint8_t buffer_size;  //!< Length of the buffer that was instanciated at runtime
    void * buff_ptr;      //!< Pointer to the allocated memory
} newqueue_t;


ret_t newqueue_init(newqueue_t * newqueue, uint8_t num_elements, uint8_t item_size);
ret_t newqueue_deinit(newqueue_t * newqueue);
ret_t newqueue_pop(newqueue_t * queue, void * data_ptr, bool peak);
ret_t newqueue_push(newqueue_t * queue, void * data_ptr);


#endif /* _NEWQUEUE_H_ */
