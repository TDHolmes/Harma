/*
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "reports.h"


#define READ_BUFF_SIZE (255)


typedef enum {
    kRead_rpt,
    kRead_len,
    kRead_payload,
    kEvaluate,
    kPrint
} cli_state_t;


typedef struct {
    ret_t (*putchr)(uint8_t);
    ret_t (*getchr)(uint8_t *);
    cli_state_t state;
    uint8_t read_buff[READ_BUFF_SIZE];
} cli_t;


cli_t cli;


ret_t cli_init(ret_t (*putchr)(uint8_t), ret_t (*getchr)(uint8_t *))
{
    cli.putchr = putchr;
    cli.getchr = getchr;
    cli.state = kRead_rpt;

    return RET_OK;
}


ret_t cli_run(void)
{
    uint8_t chr;
    ret_t retval = RET_GEN_ERR;

    // report buffers and variables
    uint8_t rpt_type = 0;
    uint8_t * rpt_out_buff_ptr = 0;
    uint8_t rpt_in_buff_len = 0;
    uint8_t rpt_out_buff_len = 0;
    static uint8_t payload_readin_ind = 0;

    switch (cli.state) {
        case kRead_rpt:
            retval = cli.getchr(&chr);
            if (retval == RET_OK) {
                rpt_type = chr;
                cli.state = kRead_len;
            }
            break;

        case kRead_len:
            retval = cli.getchr(&chr);
            if (retval == RET_OK) {
                rpt_in_buff_len = chr;
                payload_readin_ind = 0;
                cli.state = kRead_payload;
            }
            break;

        case kRead_payload:
            retval = cli.getchr(&chr);
            if (retval == RET_OK) {
                cli.read_buff[payload_readin_ind] = chr;
                payload_readin_ind += 1;

                if (payload_readin_ind == rpt_in_buff_len - 1) {
                    cli.state = kEvaluate;
                    payload_readin_ind = 0;
                }
            }
            break;

        case kEvaluate:
            // need to somehow call a function to handle this data
            retval = rpt_lookup(rpt_type, cli.read_buff, rpt_in_buff_len,
                                rpt_out_buff_ptr, &rpt_out_buff_len);
            break;

        case kPrint:
            // print out the results
            cli.putchr(retval);
            // if we succeeded in the report, continue on
            if (retval == RET_OK) {
                cli.putchr(rpt_out_buff_len);
                while (rpt_out_buff_len > 0) {
                    rpt_out_buff_len -= 1;
                    cli.putchr(rpt_out_buff_ptr[rpt_out_buff_len]);
                }
            }
            cli.state = kRead_rpt;

            break;
    }

    return RET_OK;
}
