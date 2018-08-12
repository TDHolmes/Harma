/*
 *
 */
#include <stdbool.h>

#include "common.h"
#include "peripherals/stm32-usb/usb_lib.h"
#include "peripherals/USB/usb_desc.h"

#include "cdc.h"


/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define CDC_SERIAL_IN_FRAME_INTERVAL  (1)
#define RX_BUFF_SIZE                  (0x40)


typedef struct {
    bool transfer_active;
    uint8_t rx_buffer[RX_BUFF_SIZE];
} cdc_t;


static cdc_t cdc_admin;


ret_t cdc_init(void)
{
    cdc_admin.transfer_active = false;

    return RET_OK;
}

/*
 * @brief This should be called by the USB driver after a CDC IN tranfer is finished.
 */
void cdc_inTransfer_completeCB(void)
{
    cdc_admin.transfer_active = false;
}


bool cdc_inTransferBusy(void)
{
    return cdc_admin.transfer_active;
}


/*
 * @brief Starts the transfer of `sendLength` bytes to the USB host.
 */
ret_t cdc_inTransfer_start(uint8_t *ptrBuffer, uint8_t sendLength)
{
    // Check bounds
    if(sendLength > PENSEL_DATA_SIZE) {
        return RET_MAX_LEN_ERR;
    }

    // Check if we have a transfer active
    if (cdc_admin.transfer_active == true) {
        return RET_BUSY_ERR;
    }

    // send  packet to PMA
    cdc_admin.transfer_active = true;
    // TODO: decouple from specific endpoint
    UserToPMABufferCopy((unsigned char*)ptrBuffer, ENDP1_TXADDR, sendLength);

    // TODO: decouple this function from endpoint nitty-gritty things?
    SetEPTxCount(ENDP1, sendLength);
    SetEPTxValid(ENDP1);

    return RET_OK;
}

/*
 * @brief prepares the CDC OUT endpoint to recieve data from the USB host
 */
void cdc_outTransfer_start(void)
{
    // TODO: figure out what is actually needed here...
    // TODO: decouple this function from endpoint nitty-gritty things?
    SetEPRxValid(ENDP3);
}

/*
 * @brief recieves `num_bytes` data from the CDC usb endpoint from the host
 */
ret_t cdc_outTransfer_receive(uint16_t rx_buff_addr, uint32_t num_bytes)
{
    PMAToUserBufferCopy(cdc_admin.rx_buffer, rx_buff_addr, num_bytes);

    return RET_OK;
}
