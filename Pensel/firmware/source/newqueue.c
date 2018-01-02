/*!
 * @file    newqueue.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   General purpose circular buffer handler that uses malloc.
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"
#include "newqueue.h"


// Some private functions for use by push/pop functions
void priv_increment_tail(volatile newqueue_t * queue_ptr);
void priv_decrement_tail(volatile newqueue_t * queue_ptr);
void priv_increment_head(volatile newqueue_t * queue_ptr);


/*!
 *
 */
ret_t newqueue_init(volatile newqueue_t * newqueue, uint32_t num_elements, uint32_t item_size)
{
    uint32_t size = (num_elements * item_size);
    newqueue->head_ind = 0;
    newqueue->tail_ind = 0;
    newqueue->unread_items = 0;
    newqueue->overwrite_count = 0;
    newqueue->item_size = item_size;
    newqueue->num_items = num_elements;
    newqueue->buffer_size = size;
    newqueue->buff_ptr = malloc(size);
    if (newqueue->buff_ptr == NULL) {
        return RET_MAX_LEN_ERR;
    }
    return RET_OK;
}

/*!
 *
 */
ret_t newqueue_deinit(volatile newqueue_t * newqueue)
{
    free(newqueue->buff_ptr);

    return RET_OK;
}


/*!
 *
 */
ret_t newqueue_pop(volatile newqueue_t * queue, void * data_ptr, uint32_t num_items, peak_t peak)
{
    // "memcpy" the data to `data_ptr` while incrementing the tail
    uint32_t original_tail = queue->tail_ind;
    uint32_t i = 0;
    for (uint32_t item = 0; item < num_items; item += 1) {
        for (uint32_t sub_i = 0; sub_i < queue->item_size; sub_i += 1) {
            ((uint8_t *)data_ptr)[i] = ((uint8_t *)queue->buff_ptr)[queue->tail_ind];
            priv_increment_tail(queue);
            i += 1;
        }
    }
    if (peak == ePeak) {
        // reset the tail, don't mess with unread count.
        queue->tail_ind = original_tail;
    } else {
        queue->unread_items -= num_items;
    }
    return RET_OK;
}


/*!
 *
 */
ret_t newqueue_push(volatile newqueue_t * queue, void * data_ptr, uint32_t num_items)
{
    // Cast the data as bytes and push it on byte by byte
    // while incrementing the head
    uint32_t i = 0;
    for (uint32_t item = 0; item < num_items; item += 1) {
        for (uint32_t sub_i = 0; sub_i < queue->item_size; sub_i += 1) {
            ((uint8_t *)queue->buff_ptr)[queue->head_ind] = ((uint8_t *)data_ptr)[i];
            priv_increment_head(queue);
            i += 1;
        }
        queue->unread_items += 1;
    }
    return RET_OK;
}


/*!
 *
 */
void priv_increment_tail(volatile newqueue_t * queue_ptr)
{
    if (queue_ptr->tail_ind < queue_ptr->buffer_size - 1) {
        queue_ptr->tail_ind += 1;
    } else {
        queue_ptr->tail_ind = 0;
    }
}


/*!
 *
 */
void priv_decrement_tail(volatile newqueue_t * queue_ptr)
{
    if (queue_ptr->tail_ind > 0) {
        queue_ptr->tail_ind -= 1;
    } else {
        queue_ptr->tail_ind = queue_ptr->buffer_size - 1;
    }
}


/*!
 *
 */
void priv_increment_head(volatile newqueue_t * queue_ptr)
{
    if (queue_ptr->head_ind < queue_ptr->buffer_size - 1) {
        queue_ptr->head_ind += 1;
    } else {
        queue_ptr->head_ind = 0;
    }

    // check if we're overwriting data...
    if ( !(queue_ptr->unread_items < queue_ptr->buffer_size) ) {
        // Oh no! we don't have any space. packet gets dropped :'(
        queue_ptr->overwrite_count += 1;
        // move tail ahead too since we've overwritten data
        if (queue_ptr->tail_ind < queue_ptr->buffer_size - 1) {
            queue_ptr->tail_ind += 1;
        } else {
            queue_ptr->tail_ind = 0;
        }
    }
}
