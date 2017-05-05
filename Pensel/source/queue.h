#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>


typedef struct {
    uint8_t head_ind;
    uint8_t tail_ind;
    uint8_t unread_items;
    uint8_t overwrite_count;
} queue_t;


void queue_increment_tail(queue_t * queue_admin_ptr, uint8_t queue_size);
void queue_increment_head(queue_t * queue_admin_ptr, uint8_t queue_size);

#endif /* _QUEUE_H_ */
