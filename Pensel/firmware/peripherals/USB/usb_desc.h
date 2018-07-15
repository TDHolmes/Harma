/**
  ******************************************************************************
  * @file    usb_desc.h
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Descriptor Header for Virtual COM Port Device
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


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESC_TYPE          (0x01)
#define USB_CONFIGURATION_DESC_TYPE   (0x02)
#define USB_STRING_DESC_TYPE          (0x03)
#define USB_INTERFACE_DESC_TYPE       (0x04)
#define USB_ENDPOINT_DESC_TYPE        (0x05)

#define HARMA_DATA_SIZE               (64)
#define HARMA_INT_SIZE                (8)

#define HARMA_SIZ_DEVICE_DESC         (18)
#define HARMA_SIZ_CONFIG_DESC         (107)
#define HARMA_SIZ_STRING_LANGID       (4)
#define HARMA_SIZ_STRING_VENDOR       (38)
#define HARMA_SIZ_STRING_PRODUCT      (46)
#define HARMA_SIZ_STRING_SERIAL       (26)

#define STANDARD_ENDPOINT_DESC_SIZE   (0x09)

/* Exported functions ------------------------------------------------------- */
extern const uint8_t harma_DeviceDescriptor[HARMA_SIZ_DEVICE_DESC];
extern const uint8_t harma_ConfigDescriptor[HARMA_SIZ_CONFIG_DESC];

extern const uint8_t harma_StringLangID[HARMA_SIZ_STRING_LANGID];
extern const uint8_t harma_StringVendor[HARMA_SIZ_STRING_VENDOR];
extern const uint8_t harma_StringProduct[HARMA_SIZ_STRING_PRODUCT];
extern uint8_t harma_StringSerial[HARMA_SIZ_STRING_SERIAL];

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
