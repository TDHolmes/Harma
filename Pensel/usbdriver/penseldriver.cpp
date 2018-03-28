#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define VERSION "0.1.0"
#define VENDOR_ID 0x0925   // TODO: fix for pensel
#define PRODUCT_ID 0x7001  // TODO: fix for pensel

// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B

#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

#define CTRL_IN     (LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE)
#define CTRL_OUT    (LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE)


const static int PACKET_CTRL_LEN = 2;
const static int PACKET_INT_LEN = 2;
const static int ENDPOINT_INT_IN = 0x81; /* endpoint 0x81 address for IN */
const static int ENDPOINT_INT_OUT = 0x01; /* endpoint 1 address for OUT */
const static int TIMEOUT = 5000; /* timeout in ms */

static struct libusb_device_handle *devh = NULL;

static int find_lvr_hidusb(void)
{
    devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (devh == NULL) {
        return -EIO;
    } else {
        return 0;
    }
}


static int cleanup(int retval) {
    libusb_close(devh);
    libusb_exit(NULL);
    return retval >= 0 ? retval : -retval;
}

// ---- Some testing functions ---- //

static int test_control_transfer(void)
{
    int r,i;
    unsigned char answer[PACKET_CTRL_LEN];
    unsigned char question[PACKET_CTRL_LEN];
    for (i=0; i < PACKET_CTRL_LEN; i++)  {
        question[i] = 0x20 + i;
    }

    r = libusb_control_transfer(
        devh, CTRL_OUT, HID_SET_REPORT, (HID_REPORT_TYPE_FEATURE << 8) | 0x00,
        0, question, PACKET_CTRL_LEN, TIMEOUT);
    if (r < 0) {
        fprintf(stderr, "Control Out error %d\n", r);
        return r;
    }
    r = libusb_control_transfer(
        devh, CTRL_IN, HID_GET_REPORT, (HID_REPORT_TYPE_FEATURE << 8) | 0x00,
        0, answer, PACKET_CTRL_LEN, TIMEOUT);
    if (r < 0) {
        fprintf(stderr, "Control IN error %d\n", r);
        return r;
    }
    for(i = 0; i < PACKET_CTRL_LEN; i++) {
        if(i % 8 == 0) {
            printf("\n");
            printf("%02x, %02x; ",question[i],answer[i]);
        }
    }
    printf("\n");

    return 0;
}


static int test_control_transfer_in_out(void)
{
    int r,i;
    unsigned char answer[PACKET_INT_LEN];
    unsigned char question[PACKET_INT_LEN];
    for (i=0; i < PACKET_INT_LEN; i++) {
        question[i]=0x30+i;
        answer[i] = 0;
    }

    r = libusb_control_transfer(devh,CTRL_OUT,HID_SET_REPORT,(HID_REPORT_TYPE_OUTPUT<<8)|0x00, 0,question, PACKET_INT_LEN,TIMEOUT);
    if (r < 0) {
        fprintf(stderr, "Control Out error %d\n", r);
        return r;
    }
    r = libusb_control_transfer(devh,CTRL_IN,HID_GET_REPORT,(HID_REPORT_TYPE_INPUT<<8)|0x00, 0, answer,PACKET_INT_LEN, TIMEOUT);
    if (r < 0) {
        fprintf(stderr, "Control IN error %d\n", r);
        return r;
    }
    for (i = 0;i < PACKET_INT_LEN; i++) {
        if(i%8 == 0) {
            printf("\n");
            printf("%02x, %02x; ",question[i],answer[i]);
        }
    }
    printf("\n");

    return 0;
}


static int test_interrupt_transfer(void)
{
    int r,i;
    int transferred;
    unsigned char answer[PACKET_INT_LEN];
    unsigned char question[PACKET_INT_LEN];

    for (i=0; i < PACKET_INT_LEN; i++) {
        question[i] = i;
    }

    r = libusb_interrupt_transfer(devh, ENDPOINT_INT_OUT, question, PACKET_INT_LEN,
    &transferred,TIMEOUT);
    if (r < 0) {
        fprintf(stderr, "Interrupt write error %d\n", r);
        return r;
    }
    r = libusb_interrupt_transfer(devh, ENDPOINT_INT_IN, answer, PACKET_INT_LEN,
        &transferred, TIMEOUT);
    if (r < 0) {
        fprintf(stderr, "Interrupt read error %d\n", r);
        return r;
    }
    if (transferred < PACKET_INT_LEN) {
        fprintf(stderr, "Interrupt transfer short read (%d)\n", r);
        return -1;
    }

    for (i = 0;i < PACKET_INT_LEN; i++) {
        if (i%8 == 0) {
            printf("\n");
            printf("%02x, %02x; ",question[i],answer[i]);
        }
    }
    printf("\n");
    return 0;
}


// ---- MAIN ---- //

int main(void)
{
    int r = 1;

    r = libusb_init(NULL);
    if (r < 0) {
      fprintf(stderr, "Failed to initialise libusb\n");
      exit(1);
    }

    r = find_lvr_hidusb();
    if (r < 0) {
      fprintf(stderr, "Could not find/open LVR Generic HID device\n");
      return cleanup(r);
    }
    printf("Successfully find the LVR Generic HID device\n");

#ifdef LINUX
    libusb_detach_kernel_driver(devh, 0);
#endif

    r = libusb_set_configuration(devh, 1);
    if (r < 0) {
      fprintf(stderr, "libusb_set_configuration error %d\n", r);
      return cleanup(r);
    }
    printf("Successfully set usb configuration 1\n");
    r = libusb_claim_interface(devh, 0);
    if (r < 0) {
      fprintf(stderr, "libusb_claim_interface error %d\n", r);
      return cleanup(r);
    }
    printf("Successfully claimed interface\n");

    printf("Testing control transfer using loop back test of feature report");
    test_control_transfer();
    printf("Testing control transfer using loop back test of input/output report");
    test_control_transfer_in_out();
    printf("Testing interrupt transfer using loop back test of input/output report");
    test_interrupt_transfer();

    libusb_release_interface(devh, 0);

    return cleanup(0);
}
