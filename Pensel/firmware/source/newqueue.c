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
void newqueue_increment_tail(volatile newqueue_t * queue_ptr);
void newqueue_increment_head(volatile newqueue_t * queue_ptr);


/*!
 *
 */
ret_t newqueue_init(newqueue_t * newqueue, uint8_t num_elements, uint8_t item_size)
{
    uint8_t size = (num_elements * item_size);
    newqueue->head_ind = 0;
    newqueue->tail_ind = 0;
    newqueue->unread_items = 0;
    newqueue->overwrite_count = 0;
    newqueue->item_size = item_size;
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
ret_t newqueue_deinit(newqueue_t * newqueue)
{
    free(newqueue->buff_ptr);

    return RET_OK;
}


/*!
 *
 */
ret_t newqueue_pop(newqueue_t * queue, void * data_ptr, bool peak)
{
    // "memcpy" the data to `data_ptr` while incrementing the tail
    for (uint8_t i = queue->item_size; i != 0; i -= 1) {
        *(uint8_t *)data_ptr = ((uint8_t *)queue->buff_ptr)[queue->tail_ind];
        if (peak == false) {
            newqueue_increment_tail(queue);
        }
    }
    return RET_OK;
}


/*!
 *
 */
ret_t newqueue_push(newqueue_t * queue, void * data_ptr)
{
    // Cast the data as bytes and push it on byte by byte
    // while incrementing the head
    for (uint8_t i = queue->item_size; i != 0; i -= 1) {
        ((uint8_t *)queue->buff_ptr)[queue->head_ind] = *(uint8_t *)data_ptr;
        newqueue_increment_head(queue);
    }
    return RET_OK;
}


/*!
 *
 */
void newqueue_increment_tail(volatile newqueue_t * queue_ptr)
{
    queue_ptr->unread_items -= 1;

    if (queue_ptr->tail_ind < queue_ptr->buffer_size - 1) {
        queue_ptr->tail_ind += 1;
    } else {
        queue_ptr->tail_ind = 0;
    }
}


/*!
 *
 */
void newqueue_increment_head(volatile newqueue_t * queue_ptr)
{
    if (queue_ptr->head_ind < queue_ptr->buffer_size - 1) {
        queue_ptr->head_ind += 1;
    } else {
        queue_ptr->head_ind = 0;
    }

    // check if we're overwriting data...
    if (queue_ptr->unread_items < queue_ptr->buffer_size) {
        queue_ptr->unread_items += 1;
    } else {
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
