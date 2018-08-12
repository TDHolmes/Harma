/**
 ******************************************************************************
 * @file    usb_prop.h
 * @author  MCD Application Team
 * @version V4.1.0
 * @date    26-May-2017
 * @brief   All processing related to Virtual COM Port Demo (Endpoint 0)
 ******************************************************************************
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
 ******************************************************************************
 */

#pragma once

#include <stdint.h>

#include "peripherals/stm32-usb/usb_core.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint32_t bitrate;
    uint8_t format;
    uint8_t paritytype;
    uint8_t datatype;
} LINE_CODING;

typedef enum {
    GET_REPORT = 1,
    GET_IDLE,
    GET_PROTOCOL,

    SET_REPORT = 9,
    SET_IDLE,
    SET_PROTOCOL
} HID_REQUESTS;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define penselUSB_getConfiguration NOP_Process
//#define penselUSB_SetConfiguration          NOP_Process
#define penselUSB_getInterface NOP_Process
#define penselUSB_SetInterface NOP_Process
#define penselUSB_getStatus NOP_Process
#define penselUSB_ClearFeature NOP_Process
#define penselUSB_SetEndPointFeature NOP_Process
#define penselUSB_SetDeviceFeature NOP_Process
//#define penselUSB_SetDeviceAddress          NOP_Process

#define SEND_ENCAPSULATED_COMMAND 0x00
#define GET_ENCAPSULATED_RESPONSE 0x01
#define SET_COMM_FEATURE 0x02
#define GET_COMM_FEATURE 0x03
#define CLEAR_COMM_FEATURE 0x04
#define SET_LINE_CODING 0x20
#define GET_LINE_CODING 0x21
#define SET_CONTROL_LINE_STATE 0x22
#define SEND_BREAK 0x23

/* Exported functions ------------------------------------------------------- */
void penselUSB_init(void);
void penselUSB_reset(void);
void penselUSB_SetConfiguration(void);
void penselUSB_SetDeviceAddress(void);
void penselUSB_statusIn(void);
void penselUSB_statusOut(void);
RESULT penselUSB_dataSetup(uint8_t);
RESULT penselUSB_noDataSetup(uint8_t);
RESULT penselUSB_getInterfaceSetting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *penselUSB_getDeviceDescriptor(uint16_t x);
uint8_t *penselUSB_getConfigDescriptor(uint16_t z);
uint8_t *penselUSB_getStringDescriptor(uint16_t x);

// HID stuff
uint8_t *penselHID_GetReportDescriptor(uint16_t Length);
uint8_t *penselHID_GetHIDDescriptor(uint16_t Length);
uint8_t *penselHID_GetProtocolValue(uint16_t Length);
RESULT CustomHID_SetProtocol(void);
uint8_t penselHID_setReportExecute(uint8_t report_id, uint8_t *payload_ptr, uint16_t payload_len);

// CDC stuff
uint8_t *penselUSB_getLineCoding(uint16_t Length);
uint8_t *penselUSB_SetLineCoding(uint16_t Length);

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
