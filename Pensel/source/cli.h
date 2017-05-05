#ifndef _CLI_H_
#define _CLI_H_

#include <stdint.h>
#include "common.h"


ret_t cli_init(ret_t (*putchr)(uint8_t), ret_t (*getchr)(uint8_t *));
ret_t cli_run(void);


#endif /* _CLI_H_ */
