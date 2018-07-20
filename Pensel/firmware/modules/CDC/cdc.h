
#include <stdint.h>

#include "common.h"



ret_t cdc_init(void);

void cdc_inTransfer_completeCB(void);
bool cdc_inTransferBusy(void);
ret_t cdc_inTransfer_start(uint8_t *ptrBuffer, uint8_t sendLength);
ret_t cdc_outTransfer_receive(uint16_t rx_buff_addr, uint32_t num_bytes);
