#ifndef _COMMON_H_
#define _COMMON_H_

typedef enum {
    RET_OK,
    RET_VAL_ERR,
    RET_NODATA_ERR,
    RET_LEN_ERR,
    RET_COM_ERR,
    RET_BUSY_ERR
} ret_t;


void fatal_error_handler(void);


#endif /* _COMMON_H_ */
