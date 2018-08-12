/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Descriptors for Virtual Com Port Demo
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


/* Includes ------------------------------------------------------------------*/
#include "peripherals/stm32-usb/usb_lib.h"
#include "usb_desc.h"


#define USB_DEVICE_CLASS_MISC               (0xEF)
#define USB_IFACE_ASSOCIATION_DESC_SIZE     (0x08)
#define USB_IFACE_ASSOCIATION_DESC_TYPE     (0x0B)
#define USB_DEVICE_CLASS_COMMUNICATIONS     (0x02)
#define CDC_ABSTRACT_CONTROL_MODEL          (0x02)
#define CDC_AT_COMMANDS                     (0x01)
#define HID_DESC_TYPE                       (0x03)


/* USB Standard Device Descriptor */
const uint8_t pensel_DeviceDescriptor[] = {

    0x12,                       // bLength              Descriptor size (18 bytes)
    USB_DEVICE_DESC_TYPE,       // bDescriptorType      Descriptor type (Device)
    0x00, 0x02,                 // bcdUSB               USB spec 2.0
    USB_DEVICE_CLASS_MISC,      // bDeviceClass         Use interface Association descriptors
    0x02,                       // bDeviceSubClass      to make a composite device
    0x01,                       // bDeviceProtocol      ''
    0x40,                       // bMaxPacketSize0      Max packet size on iface 0
    0x83, 0x04,                 // idVendor             Vendor ID (0x0483)
    0x40, 0x57,                 // idProduct            Product ID (0x7540)
    0x00, 0x02,                 // bcdDevice            Device release number (2.00)
    0x01,                       // iManufacturer        Index of string manufacturer descriptor
    0x02,                       // iProduct             Index of string describing product
    0x03,                       // iSerialNumber        Index of string for device's serial number
    0x01                        // bNumConfigurations
  };

const uint8_t pensel_ConfigDescriptor[] = {

    // --- Configuration Descriptor
    0x09,                           // bLength              Configuration Descriptor size
    USB_CONFIGURATION_DESC_TYPE,    // bDescriptorType      Configuration
    PENSEL_SIZ_CONFIG_DESC,          // wTotalLength         no of returned bytes in descriptor
    0x00,                           //                      and subordinate descriptors
    0x03,                           // bNumInterfaces       3 interfaces (2 CDC 1 HID)
    0x01,                           // bConfigurationValue  Configuration value
    0x00,                           // iConfiguration       Index of string describing configuration
    0xC0,                           // bmAttributes         self powered  //TODO: fix
    0x32,                           // MaxPower             100 mA (increase to 300 somehow?)

    // --- Virtual Com Port Device --- //

    // --- Interface Association Descriptor (for the two virtual com port interfaces)
    USB_IFACE_ASSOCIATION_DESC_SIZE,    // bLength
    USB_IFACE_ASSOCIATION_DESC_TYPE,    // bDescriptorType
    0,                                  // bFirstInterface
    2,                                  // bInterfaceCount
    USB_DEVICE_CLASS_COMMUNICATIONS,    // bFunctionClass
    CDC_ABSTRACT_CONTROL_MODEL,         // bFunctionSubClass
    CDC_AT_COMMANDS,                    // bFunctionProtocol
    0,                                  // iFunction            Index of string descriptor

    // --- Interface Descriptor (CDC control iface)
    0x09,                           // bLength              Interface Descriptor size
    USB_INTERFACE_DESC_TYPE,        // bDescriptorType      Interface

    //  Interface descriptor type
    0x00,                           // bInterfaceNumber     Number of Interface
    0x00,                           // bAlternateSetting    Alternate setting
    0x01,                           // bNumEndpoints        One endpoints used
    0x02,                           // bInterfaceClass      CDC-Control
    0x02,                           // bInterfaceSubClass   Abstract Control Model
    0x01,                           // bInterfaceProtocol   Common AT commands
    0x00,                           // iInterface           Index of interface string descriptor

    // Header Functional Descriptor
    0x05,                           // bLength              Endpoint Descriptor size
    0x24,                           // bDescriptorType      CS_INTERFACE
    0x00,                           // bDescriptorSubtype   Header Func Desc
    0x10, 0x01,                     // bcdCDC               spec release number (1.10)

    // Call Management Functional Descriptor
    0x05,                           // bFunctionLength
    0x24,                           // bDescriptorType      CS_INTERFACE
    0x01,                           // bDescriptorSubtype   Call Management Func Desc
    0x00,                           // bmCapabilities       D0+D1
    0x01,                           // bDataInterface       1

    // ACM Functional Descriptor
    0x04,                           // bFunctionLength
    0x24,                           // bDescriptorType      CS_INTERFACE
    0x02,                           // bDescriptorSubtype   Abstract Control Management desc
    0x02,                           // bmCapabilities       ? TODO figure out

    // Union Functional Descriptor
    0x05,                           // bFunctionLength
    0x24,                           // bDescriptorType      CS_INTERFACE
    0x06,                           // bDescriptorSubtype   Union func desc
    0x00,                           // bMasterInterface     Communication class interface
    0x01,                           // bSlaveInterface0     Data Class Interface

    // --- Endpoint 2 Descriptor
    0x07,                           // bLength              Endpoint Descriptor size
    USB_ENDPOINT_DESC_TYPE,         // bDescriptorType      Endpoint
    0x82,                           // bEndpointAddress     (IN2)
    0x03,                           // bmAttributes         Interrupt
    PENSEL_INT_SIZE, 0x00,          // wMaxPacketSize
    0xFF,                           // bInterval

    // --- Interface Descriptor (CDC data iface)
    0x09,                           // bLength              Endpoint Descriptor size
    USB_INTERFACE_DESC_TYPE,        // bDescriptorType:
    0x01,                           // bInterfaceNumber     Number of Interface
    0x00,                           // bAlternateSetting    Alternate setting
    0x02,                           // bNumEndpoints        Two endpoints used
    0x0A,                           // bInterfaceClass      CDC-Data
    0x00,                           // bInterfaceSubClass   N/A
    0x00,                           // bInterfaceProtocol   N/A
    0x00,                           // iInterface

    // --- Endpoint 3 Descriptor
    0x07,                           // bLength              Endpoint Descriptor size
    USB_ENDPOINT_DESC_TYPE,         // bDescriptorType      Endpoint
    0x03,                           // bEndpointAddress     (OUT3)
    0x02,                           // bmAttributes         Bulk
    PENSEL_DATA_SIZE, 0x00,         // wMaxPacketSize
    0x00,                           // bInterval            ignore for Bulk transfer

    // --- Endpoint 1 Descriptor
    0x07,                           // bLength              Endpoint Descriptor size
    USB_ENDPOINT_DESC_TYPE,         // bDescriptorType      Endpoint
    0x81,                           // bEndpointAddress     (IN1)
    0x02,                           // bmAttributes         Bulk
    PENSEL_DATA_SIZE, 0x00,         // wMaxPacketSize
    0x00,                           // bInterval

    // --- Custom HID Device --- //

    // --- Interface Association Descriptor (for the two virtual com port interfaces)
    USB_IFACE_ASSOCIATION_DESC_SIZE,    // bLength
    USB_IFACE_ASSOCIATION_DESC_TYPE,    // bDescriptorType
    2,                                  // bFirstInterface
    1,                                  // bInterfaceCount
    HID_DESC_TYPE,                      // bFunctionClass
    0,                                  // bFunctionSubClass
    0,                                  // bFunctionProtocol
    0,                                  // iFunction            Index of string descriptor

    // --- Interface Descriptor (HID device)

    0x09,                           // bLength              Interface Descriptor size
    USB_INTERFACE_DESC_TYPE,        // bDescriptorType      Interface descriptor type
    0x02,                           // bInterfaceNumber     Number of Interface
    0x00,                           // bAlternateSetting    Alternate setting
    0x02,                           // bNumEndpoints
    HID_DESC_TYPE,                  // bInterfaceClass      HID
    0x00,                           // bInterfaceSubClass   1=BOOT  0=no boot
    0x00,                           // nInterfaceProtocol   0=none  1=keyboard  2=mouse
    0,                              // iInterface           Index of string descriptor

    // HID descriptor

    0x09,                           // bLength              HID Descriptor size
    HID_DESCRIPTOR_TYPE,            // bDescriptorType      HID
    0x10, 0x01,                     // bcdHID               HID Class Spec release number
    0x00,                           // bCountryCode         Hardware target country
    0x01,                           // bNumDescriptors      Number of HID class descriptors to follow
    0x22,                           // bDescriptorType
    PENSEL_HID_SIZ_REPORT_DESC,     // wItemLength          Total length of Report descriptor
    0x00,

    // --- Endpoint 4 Descriptor
    0x07,                           // bLength              Endpoint Descriptor size
    USB_ENDPOINT_DESC_TYPE,         // bDescriptorType
    0x84,                           // bEndpointAddress     Endpoint Address (IN4)
    0x03,                           // bmAttributes         Interrupt endpoint
    0x02, 0x00,                     // wMaxPacketSize       2 Bytes max // TODO: figure out
    0x20,                           // bInterval            Polling Interval (X ms)  // TODO: figure out

    // --- Endpoint 5 Descriptor  (NOT NEEDED - no interrupt output reports required)
    0x07,	                        // bLength                Endpoint Descriptor size
    USB_ENDPOINT_DESC_TYPE,	        // bDescriptorType        Endpoint descriptor type
    0x05,                           // bEndpointAddress       Endpoint Address (OUT5)
    0x03,	                        // bmAttributes           Interrupt endpoint
    0x02, 0x00,                     // wMaxPacketSize         2 Bytes max // TODO: figure out
    0x20,	                        // bInterval              Polling Interval (X ms)  // TODO: figure out
};


const uint8_t penselHID_ReportDescriptor[PENSEL_HID_SIZ_REPORT_DESC] = {
    0x06, 0xFF, 0x00,      // USAGE_PAGE    (Vendor Page: 0xFF00)
    0x09, 0x01,            // USAGE         (Demo Kit)
    0xa1, 0x01,            // COLLECTION    (Application)

    // Led 1
    0x85, 0x01,            //     REPORT_ID (1)
    0x09, 0x01,            //     USAGE (LED 1)
    0x15, 0x00,            //     LOGICAL_MINIMUM (0)
    0x25, 0x01,            //     LOGICAL_MAXIMUM (1)
    0x75, 0x08,            //     REPORT_SIZE (8)
    0x95, 0x01,            //     REPORT_COUNT (1)
    0xB1, 0x82,            //     FEATURE (Data,Var,Abs,Vol)

    0x85, 0x01,            //     REPORT_ID (1)
    0x09, 0x01,            //     USAGE (LED 1)
    0x91, 0x82,            //     OUTPUT (Data,Var,Abs,Vol)

    // Led 2
    0x85, 0x02,            //     REPORT_ID 2
    0x09, 0x02,            //     USAGE (LED 2)
    0x15, 0x00,            //     LOGICAL_MINIMUM (0)
    0x25, 0x01,            //     LOGICAL_MAXIMUM (1)
    0x75, 0x08,            //     REPORT_SIZE (8)
    0x95, 0x01,            //     REPORT_COUNT (1)
    0xB1, 0x82,             //    FEATURE (Data,Var,Abs,Vol)

    0x85, 0x02,            //     REPORT_ID (2)
    0x09, 0x02,            //     USAGE (LED 2)
    0x91, 0x82,            //     OUTPUT (Data,Var,Abs,Vol)

    // Led 3
    0x85, 0x03,            //     REPORT_ID (3)
    0x09, 0x03,            //     USAGE (LED 3)
    0x15, 0x00,            //     LOGICAL_MINIMUM (0)
    0x25, 0x01,            //     LOGICAL_MAXIMUM (1)
    0x75, 0x08,            //     REPORT_SIZE (8)
    0x95, 0x01,            //     REPORT_COUNT (1)
    0xB1, 0x82,             //    FEATURE (Data,Var,Abs,Vol)

    0x85, 0x03,            //     REPORT_ID (3)
    0x09, 0x03,            //     USAGE (LED 3)
    0x91, 0x82,            //     OUTPUT (Data,Var,Abs,Vol)

    // key Push Button
    0x85, 0x05,            //     REPORT_ID (5)
    0x09, 0x05,            //     USAGE (Push Button)
    0x15, 0x00,            //     LOGICAL_MINIMUM (0)
    0x25, 0x01,            //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,            //     REPORT_SIZE (1)
    0x81, 0x82,            //     INPUT (Data,Var,Abs,Vol)

    0x09, 0x05,            //     USAGE (Push Button)
    0x75, 0x01,            //     REPORT_SIZE (1)
    0xb1, 0x82,            //     FEATURE (Data,Var,Abs,Vol)

    0x75, 0x07,            //     REPORT_SIZE (7)
    0x81, 0x83,            //     INPUT (Cnst,Var,Abs,Vol)
    0x85, 0x05,            //     REPORT_ID (2)

    0x75, 0x07,            //     REPORT_SIZE (7)
    0xb1, 0x83,            //     FEATURE (Cnst,Var,Abs,Vol)

    // Tamper Push Button
    0x85, 0x06,            //     REPORT_ID (6)
    0x09, 0x06,            //     USAGE (Tamper Push Button)
    0x15, 0x00,            //     LOGICAL_MINIMUM (0)
    0x25, 0x01,            //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,            //     REPORT_SIZE (1)
    0x81, 0x82,            //     INPUT (Data,Var,Abs,Vol)

    0x09, 0x06,            //     USAGE (Tamper Push Button)
    0x75, 0x01,            //     REPORT_SIZE (1)
    0xb1, 0x82,            //     FEATURE (Data,Var,Abs,Vol)

    0x75, 0x07,            //     REPORT_SIZE (7)
    0x81, 0x83,            //     INPUT (Cnst,Var,Abs,Vol)
    0x85, 0x06,            //     REPORT_ID (6)

    0x75, 0x07,            //     REPORT_SIZE (7)
    0xb1, 0x83,            //     FEATURE (Cnst,Var,Abs,Vol)

    // ADC IN
    0x85, 0x07,            //     REPORT_ID (7)
    0x09, 0x07,            //     USAGE (ADC IN)
    0x15, 0x00,            //     LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,      //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,            //     REPORT_SIZE (8)
    0x81, 0x82,            //     INPUT (Data,Var,Abs,Vol)
    0x85, 0x07,            //     REPORT_ID (7)
    0x09, 0x07,            //     USAGE (ADC in)
    0xb1, 0x82,            //     FEATURE (Data,Var,Abs,Vol)

    0xc0 	          //     END_COLLECTION
}; /* CustomHID_ReportDescriptor */


/* USB String Descriptors */
const uint8_t pensel_StringLangID[PENSEL_SIZ_STRING_LANGID] =
  {
    PENSEL_SIZ_STRING_LANGID,
    USB_STRING_DESC_TYPE,
    0x09, 0x04                     // LangID = 0x0409: U.S. English
  };

const uint8_t pensel_StringVendor[PENSEL_SIZ_STRING_VENDOR] =
  {
    PENSEL_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    USB_STRING_DESC_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "Holmes Engineering" */
    'H', 0, 'o', 0, 'l', 0, 'm', 0, 'e', 0, 's', 0, ' ', 0,
    'E', 0, 'n', 0, 'g', 0, 'i', 0, 'n', 0, 'e', 0, 'e', 0, 'r', 0,
    'i', 0, 'n', 0, 'g', 0
  };

const uint8_t pensel_StringProduct[PENSEL_SIZ_STRING_PRODUCT] =
  {
    PENSEL_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESC_TYPE,        /* bDescriptorType */
    /* Product name: "Pensel" */
    'P', 0, 'e', 0, 'n', 0, 's', 0, 'e', 0, 'l', 0
  };

uint8_t pensel_StringSerial[PENSEL_SIZ_STRING_SERIAL] =
  {
    PENSEL_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESC_TYPE,                   /* bDescriptorType */
    // Rest initialized in hw_config.c...
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
