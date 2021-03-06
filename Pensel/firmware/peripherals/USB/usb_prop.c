/**
 ******************************************************************************
 * @file    usb_prop.c
 * @author  MCD Application Team / TDH
 * @version V4.1.0
 * @date    26-May-2017
 * @brief   All processing related to USB properties
 * Copyright at bottom
 *****************************************************************************/

#include "usb_prop.h"
#include "hw_config.h"
#include "modules/HID/hid.h"
#include "modules/utilities/logging.h"
#include "peripherals/stm32-usb/usb_def.h"
#include "peripherals/stm32-usb/usb_lib.h"
#include "usb_conf.h"
#include "usb_desc.h"
#include "usb_pwr.h"

#define MAX_REPORT_LEN (64)
#define REPORT_DESCRIPTOR (0x22)

uint8_t Request = 0;    // TODO: figure out function
uint32_t ProtocolValue; // TODO: figure out function

static uint8_t report_buffer[MAX_REPORT_LEN];

LINE_CODING linecoding = {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
};

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table = {EP_NUM, 1};

DEVICE_PROP Device_Property = {
    penselUSB_init,
    penselUSB_reset,
    penselUSB_statusIn,
    penselUSB_statusOut,
    penselUSB_dataSetup,
    penselUSB_noDataSetup,
    penselUSB_getInterfaceSetting,
    penselUSB_getDeviceDescriptor,
    penselUSB_getConfigDescriptor,
    penselUSB_getStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
    penselUSB_getConfiguration,   penselUSB_SetConfiguration, penselUSB_getInterface,
    penselUSB_SetInterface,       penselUSB_getStatus,        penselUSB_ClearFeature,
    penselUSB_SetEndPointFeature, penselUSB_SetDeviceFeature, penselUSB_SetDeviceAddress};

ONE_DESCRIPTOR Device_Descriptor = {(uint8_t *)pensel_DeviceDescriptor, PENSEL_SIZ_DEVICE_DESC};

ONE_DESCRIPTOR Config_Descriptor = {(uint8_t *)pensel_ConfigDescriptor, PENSEL_SIZ_CONFIG_DESC};

// HID specific ones

ONE_DESCRIPTOR HID_ReportDescriptor = {(uint8_t *)penselHID_ReportDescriptor,
                                       PENSEL_HID_SIZ_REPORT_DESC};

ONE_DESCRIPTOR penselHID_HIDDescriptor = {
    (uint8_t *)(pensel_ConfigDescriptor + PENSEL_CONFIG_DESC_HID_OFFSET),
    PENSEL_CONFIG_DESC_HID_SIZE};

ONE_DESCRIPTOR String_Descriptor[4] = {{(uint8_t *)pensel_StringLangID, PENSEL_SIZ_STRING_LANGID},
                                       {(uint8_t *)pensel_StringVendor, PENSEL_SIZ_STRING_VENDOR},
                                       {(uint8_t *)pensel_StringProduct, PENSEL_SIZ_STRING_PRODUCT},
                                       {(uint8_t *)pensel_StringSerial, PENSEL_SIZ_STRING_SERIAL}};

/*******************************************************************************
 * Function Name  : penselUSB_init.
 * Description    : Virtual COM Port Mouse init routine.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void penselUSB_init(void)
{
    LOG_MSG(kLogLevelDebug, "USB: init");
    /* Update the serial number string descriptor with the data from the uniqueID */
    Get_SerialNum();

    pInformation->Current_Configuration = 0;

    /* Connect the device */
    PowerOn();

    /* Perform basic device initialization operations */
    USB_SIL_Init();

    bDeviceState = UNCONNECTED;
}

/*******************************************************************************
 * Function Name  : penselUSB_reset
 * Description    : penselUSB Mouse reset routine
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void penselUSB_reset(void)
{
    LOG_MSG(kLogLevelDebug, "USB: reset");
    /* Set penselUSB DEVICE as not configured */
    pInformation->Current_Configuration = 0;

    /* Current Feature initialization */
    // TODO: magic number
    pInformation->Current_Feature = pensel_ConfigDescriptor[7];

    /* Set penselUSB DEVICE with the default Interface*/
    pInformation->Current_Interface = 0;

    SetBTABLE(BTABLE_ADDRESS);

    /* Initialize Endpoint 0 */
    SetEPType(ENDP0, EP_CONTROL);
    SetEPTxStatus(ENDP0, EP_TX_STALL);
    SetEPRxAddr(ENDP0, ENDP0_RXADDR);
    SetEPTxAddr(ENDP0, ENDP0_TXADDR);
    Clear_Status_Out(ENDP0);
    SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
    SetEPRxValid(ENDP0);

    /* Initialize Endpoint 1 */
    SetEPType(ENDP1, EP_BULK);
    SetEPTxAddr(ENDP1, ENDP1_TXADDR);
    SetEPTxStatus(ENDP1, EP_TX_NAK);
    SetEPRxStatus(ENDP1, EP_RX_DIS);

    /* Initialize Endpoint 2 */
    SetEPType(ENDP2, EP_INTERRUPT);
    SetEPTxAddr(ENDP2, ENDP2_TXADDR);
    SetEPRxStatus(ENDP2, EP_RX_DIS);
    SetEPTxStatus(ENDP2, EP_TX_NAK);

    /* Initialize Endpoint 3 */
    SetEPType(ENDP3, EP_BULK);
    SetEPRxAddr(ENDP3, ENDP3_RXADDR);
    SetEPRxCount(ENDP3, PENSEL_DATA_SIZE);
    SetEPRxStatus(ENDP3, EP_RX_VALID);
    SetEPTxStatus(ENDP3, EP_TX_DIS);

    /* Initialize Endpoint 4 */
    // SetEPType(ENDP4, EP_INTERRUPT);
    // SetEPRxAddr(ENDP4, ENDP4_RXADDR);
    // SetEPRxCount(ENDP4, PENSEL_DATA_SIZE);
    // SetEPRxStatus(ENDP4, EP_RX_VALID);
    // SetEPTxStatus(ENDP4, EP_TX_DIS);

    /* Set this device to response on default address */
    SetDeviceAddress(0);

    bDeviceState = ATTACHED;
}

/*******************************************************************************
 * Function Name  : penselUSB_SetConfiguration.
 * Description    : Update the device state to configured.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void penselUSB_SetConfiguration(void)
{
    LOG_MSG(kLogLevelDebug, "USB: set conn");
    DEVICE_INFO *pInfo = &Device_Info;

    if (pInfo->Current_Configuration != 0) {
        /* Device configured */
        bDeviceState = CONFIGURED;
    }
}

/*******************************************************************************
 * Function Name  : penselUSB_SetConfiguration.
 * Description    : Update the device state to addressed.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void penselUSB_SetDeviceAddress(void)
{
    LOG_MSG(kLogLevelDebug, "USB: set addn");
    bDeviceState = ADDRESSED;
}

/*******************************************************************************
 * Function Name  : penselUSB_statusIn.
 * Description    : Virtual COM Port Status In Routine.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void penselUSB_statusIn(void)
{
    ret_t ret;
    if (Request == SET_LINE_CODING) {
        LOG_MSG(kLogLevelDebug, "Status IN - SET_LINE_CODING");
        Request = 0;
    } else if (Request == GET_REPORT) {
        LOG_MSG(kLogLevelDebug, "Status IN - GET_REPORT");
        Request = 0;
    } else if (Request == SET_REPORT) {
        LOG_MSG(kLogLevelDebug, "Status IN - SET_REPORT");
        Request = 0;

        uint16_t payload_len = pInformation->USBwLengths.w;
        uint8_t report_id = pInformation->USBwValues.bw.bb0;

        ret = hid_setReport(report_id, report_buffer, payload_len);
        LOG_MSG_FMT(kLogLevelInfo, "Set Report: 0x%02X (len: %i) - Retval: %i", report_id,
                    payload_len, ret);

#define TMP_ARR_MSG_LEN (256)
        char TmpArrMsg[TMP_ARR_MSG_LEN];
        int num_bytes = 0;
        int num_chars = sprintf(TmpArrMsg, "Payload: ");
        while (1) {
            num_chars += sprintf(TmpArrMsg + num_chars, "%02X ", report_buffer[num_bytes]);
            if (num_chars >= TMP_ARR_MSG_LEN) {
                TmpArrMsg[TMP_ARR_MSG_LEN - 1] = 0;
                break;
            }
            num_bytes += 1;
            if (num_bytes >= payload_len) {
                break;
            }
        }
        LOG_MSG(kLogLevelDebug, TmpArrMsg);
    }
}

/*******************************************************************************
 * Function Name  : penselUSB_statusOut
 * Description    : Status OUT Routine.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void penselUSB_statusOut(void)
{
    // TODO: what does this do?
    LOG_MSG(kLogLevelDebug, "Status OUT");
}

uint8_t *penselHID_setReport(uint16_t Length)
{
    if (Length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
        return NULL;
    }

    if (Length > MAX_REPORT_LEN) {
        // TODO: error?
        return NULL;
    }
    return report_buffer;
}

uint8_t *penselHID_getReport(uint16_t Length)
{
    uint8_t report_id = pInformation->USBwValues.bw.bb0;
    report_buffer[0] = report_id;
    uint8_t *payload = report_buffer + 1;
    uint8_t payload_len = 0;
    ret_t ret;

    if (Length > MAX_REPORT_LEN) {
        // TODO: error?
        return NULL;
    }

    ret = hid_getReport(report_id, payload, &payload_len);
    pInformation->Ctrl_Info.Usb_wLength = payload_len + 1;
    return report_buffer;
}

/*******************************************************************************
 * Function Name  : penselUSB_dataSetup
 * Description    : handle the data class specific requests
 * Input          : Request Nb.
 * Output         : None.
 * Return         : USB_UNSUPPORT or USB_SUCCESS.
 *******************************************************************************/
RESULT penselUSB_dataSetup(uint8_t RequestNo)
{
    // TODO: CDC-specific? Move to cdc.c?
    uint8_t *(*CopyRoutine)(uint16_t);

    CopyRoutine = NULL;

    // CDC-specific requests
    if (RequestNo == GET_LINE_CODING) {
        if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
            CopyRoutine = penselUSB_getLineCoding;
        }
    } else if (RequestNo == SET_LINE_CODING) {
        if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
            CopyRoutine = penselUSB_SetLineCoding;
        }
        Request = SET_LINE_CODING;
    }

    // HID-specific requests
    if ((RequestNo == GET_DESCRIPTOR) &&
        (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))) {
        if (pInformation->USBwValue1 == REPORT_DESCRIPTOR) {
            CopyRoutine = penselHID_GetReportDescriptor;
        } else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE) {
            CopyRoutine = penselHID_GetHIDDescriptor;
        }

    } /* End of GET_DESCRIPTOR */

    /*** GET_PROTOCOL, GET_REPORT, SET_REPORT ***/
    else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))) {
        switch (RequestNo) {
            case GET_PROTOCOL:
                CopyRoutine = penselHID_GetProtocolValue;
                break;

            case SET_REPORT:
                CopyRoutine = penselHID_setReport;
                Request = SET_REPORT;
                break;

            case GET_REPORT:

                CopyRoutine = penselHID_getReport;
                Request = GET_REPORT;
                break;

            default:
                // TODO: error detection?
                break;
        }
    }

    if (CopyRoutine == NULL) {
        return USB_UNSUPPORT;
    }

    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    LOG_MSG(kLogLevelDebug, "Copy Routine call here. Zero len");
    (*CopyRoutine)(0);
    return USB_SUCCESS;
}

/*******************************************************************************
 * Function Name  : penselUSB_noDataSetup.
 * Description    : handle the no data class specific requests.
 * Input          : Request Nb.
 * Output         : None.
 * Return         : USB_UNSUPPORT or USB_SUCCESS.
 *******************************************************************************/
RESULT penselUSB_noDataSetup(uint8_t RequestNo)
{
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
        // CDC specific stuff
        if (RequestNo == SET_COMM_FEATURE) {
            return USB_SUCCESS;
        } else if (RequestNo == SET_CONTROL_LINE_STATE) {
            return USB_SUCCESS;

            // HID specific stuff
        } else if (RequestNo == SET_PROTOCOL) {
            return CustomHID_SetProtocol();
        }
    }

    return USB_UNSUPPORT;
}

/*******************************************************************************
 * Function Name  : CustomHID_SetProtocol
 * Description    : Joystick Set Protocol request routine.
 * Input          : None.
 * Output         : None.
 * Return         : USB SUCCESS.
 *******************************************************************************/
RESULT CustomHID_SetProtocol(void)
{
    uint8_t wValue0 = pInformation->USBwValue0;
    ProtocolValue = wValue0;
    return USB_SUCCESS;
}

/*******************************************************************************
 * Function Name  : CustomHID_GetProtocolValue
 * Description    : get the protocol value
 * Input          : Length.
 * Output         : None.
 * Return         : address of the protocol value.
 *******************************************************************************/
uint8_t *penselHID_GetProtocolValue(uint16_t Length)
{
    if (Length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = 1;
        return NULL;
    } else {
        return (uint8_t *)(&ProtocolValue);
    }
}

/*******************************************************************************
 * Function Name  : penselUSB_getDeviceDescriptor.
 * Description    : Gets the device descriptor.
 * Input          : Length.
 * Output         : None.
 * Return         : The address of the device descriptor.
 *******************************************************************************/
uint8_t *penselUSB_getDeviceDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
 * Function Name  : penselUSB_getConfigDescriptor.
 * Description    : get the configuration descriptor.
 * Input          : Length.
 * Output         : None.
 * Return         : The address of the configuration descriptor.
 *******************************************************************************/
uint8_t *penselUSB_getConfigDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
 * Function Name  : penselHID_GetReportDescriptor.
 * Description    : Gets the HID report descriptor.
 * Input          : Length
 * Output         : None.
 * Return         : The address of the configuration descriptor.
 *******************************************************************************/
uint8_t *penselHID_GetReportDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &HID_ReportDescriptor);
}

uint8_t *penselHID_GetHIDDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &penselHID_HIDDescriptor);
}

/*******************************************************************************
 * Function Name  : penselUSB_getStringDescriptor
 * Description    : Gets the string descriptors according to the needed index
 * Input          : Length.
 * Output         : None.
 * Return         : The address of the string descriptors.
 *******************************************************************************/
uint8_t *penselUSB_getStringDescriptor(uint16_t Length)
{
    uint8_t wValue0 = pInformation->USBwValue0;
    if (wValue0 >= 4) {
        return NULL;
    } else {
        return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
    }
}

/*******************************************************************************
 * Function Name  : penselUSB_getInterfaceSetting.
 * Description    : test the interface and the alternate setting according to the
 *                  supported one.
 * Input1         : uint8_t: Interface : interface number.
 * Input2         : uint8_t: AlternateSetting : Alternate Setting number.
 * Output         : None.
 * Return         : The address of the string descriptors.
 *******************************************************************************/
RESULT penselUSB_getInterfaceSetting(uint8_t Interface, uint8_t AlternateSetting)
{
    if (AlternateSetting > 0) {
        return USB_UNSUPPORT;
    } else if (Interface > 1) {
        return USB_UNSUPPORT;
    }
    return USB_SUCCESS;
}

/*******************************************************************************
 * Function Name  : penselUSB_getLineCoding.
 * Description    : send the linecoding structure to the PC host.
 * Input          : Length.
 * Output         : None.
 * Return         : Linecoding structure base address.
 *******************************************************************************/
uint8_t *penselUSB_getLineCoding(uint16_t Length)
{
    if (Length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
        return NULL;
    }
    return (uint8_t *)&linecoding;
}

/*******************************************************************************
 * Function Name  : penselUSB_SetLineCoding.
 * Description    : Set the linecoding structure fields.
 * Input          : Length.
 * Output         : None.
 * Return         : Linecoding structure base address.
 *******************************************************************************/
uint8_t *penselUSB_SetLineCoding(uint16_t Length)
{
    if (Length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
        return NULL;
    }
    return (uint8_t *)&linecoding;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************/
