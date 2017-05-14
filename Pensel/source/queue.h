#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>


typedef struct {
    volatile uint8_t head_ind;
    volatile uint8_t tail_ind;
    volatile uint8_t unread_items;
    volatile uint8_t overwrite_count;
} queue_t;


void queue_increment_tail(volatile queue_t * queue_admin_ptr, const uint8_t queue_size);
void queue_increment_head(volatile queue_t * queue_admin_ptr, const uint8_t queue_size);

#endif /* _QUEUE_H_ */
