/*!
 * @file    reports.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Interface to define get/set reports as a debug hook.
 *
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "reports.h"
#include "modules/LSM303DLHC/LSM303DLHC.h"
#include "modules/calibration/cal.h"
#include "modules/orientation/orientation.h"
#include "peripherals/UART/UART.h"        // TODO: Remove and switch back to function pointers
#include "peripherals/hardware/hardware.h"
#include "modules/utilities/newqueue.h"


// TODO: Is this extern declaration a good idea? Or should I include the HAL header?
extern uint32_t HAL_GetTick(void);
extern critical_errors_t gCriticalErrors;


//! The number of bytes in our report header
#define RPT_HEADER_SIZE (6)
#define MAX_RPT_SIZE (255)
//! Maximum data we can read in (plus our TX header)
#define READ_BUFF_SIZE (MAX_RPT_SIZE + RPT_HEADER_SIZE)
//! Number of input reports we're able to cache
#define NUM_INPUTS_TO_CACHE (3)

//! First magic number to specify start of report
#define RPT_MAGIC_NUMBER_0 (0xDE)
//! Second magic number to specify start of report
#define RPT_MAGIC_NUMBER_1 (0xAD)
//! Third magic number to specify start of report
#define RPT_MAGIC_NUMBER_2 (0xBE)
//! Fourth magic number to specify start of report
#define RPT_MAGIC_NUMBER_3 (0xEF)

#define RPT_TIMEOUT (100)  //!< Report timeout time (in ms)

// some bit masks to be applied on top of report IDs when replying or streaming to the host
#define RPT_REPORT_MASK (0 << 7)  //!< bitmask to specify a packet is a report
#define RPT_STREAM_MASK (1 << 7)  //!< bitmask to specify a packet is a stream

//! Can only have up to 0x7F as the first bit is used to indicate stream or report
#define MAX_REPORT_ID (0x7F)


//! The current state of the rpt workloop
typedef enum {
    //!< Read in the first magic value to signify the start of a report transaction
    kRpt_ReadMagic_0,
    //!< Read in the second magic value to signify the start of a report transaction
    kRpt_ReadMagic_1,
    //!< Read in the third magic value to signify the start of a report transaction
    kRpt_ReadMagic_2,
    //!< Read in the fourth magic value to signify the start of a report transaction
    kRpt_ReadMagic_3,
    kRpt_ReadRpt,      //!< Read the report ID (the first byte of the transaction)
    kRpt_ReadLen,      //!< Read the length of the report payload (2nd byte)
    kRpt_ReadPayload,  //!< Read the report payload (N bytes...)
    kRpt_Checksum,     //!< Red in the checksum and check if it's valid
    //! Evaluate the report on our end and send back the response (x bytes)
    kRpt_Evaluate,
    kRpt_Printing      //!< The state to send the reply out
} rpt_state_t;


//! Admin struct for the report module
typedef struct {
    ret_t (*putchr)(uint8_t);   //!< function pointer to send chars out
    ret_t (*getchr)(uint8_t *); //!< function pointer to recieve chars
    rpt_state_t state;          //!< Current state of the report module
    //! Start time from HAL_GetTick() of the active report. Times out after `RPT_TIMEOUT`
    uint32_t start_time;
    uint16_t invalid_chrs;      //!< Number of invalid bytes we've received
    uint16_t timeouts;          //!< Number of times a command times out
    uint8_t read_buff[READ_BUFF_SIZE];  //!< Buffer to read the reports in

} rpt_t;

//! Bufer to hold the reply of the report
uint8_t output_buffer[OUTPUT_BUFF_LEN];

static rpt_t rpt;
bool input_report_running = false;  //!< private flag to indicate if an input report is pending

uint8_t rpt_out_buff_len = 0;
// Keeps track of how many bytes of `rpt_out_buff_len` we've sent in state kRpt_Printing
uint8_t rpt_out_sent_ind = 0;

// private function declarations
static uint8_t priv_calcChecksum(uint8_t * data, uint32_t num_bytes);
static ret_t priv_reportLookup(
    uint8_t rpt_type, uint8_t *input_buff_ptr, uint8_t input_buff_len,
    uint8_t * output_buffer_ptr, uint8_t * output_buff_len_ptr);

/* -------------- Initializer and runner ------------------------ */

static uint8_t priv_calcChecksum(uint8_t * data, uint32_t num_bytes)
{
    uint8_t checksum = 0;
    while (num_bytes) {
        checksum += *data;
        data += 1;
        num_bytes -= 1;
    }
    return -checksum;
}

/*! Initializes the reporting interface by storing the get and put char functions and
 *    initializing the admin structure.
 *
 * @param putchr: Function pointer used to write out a character. Returns ret_t and
 *      takes a uint8_t in as a parameter.
 * @param getchr: Function pointer used to read in a character. Returns ret_t and
 *      takes a uint8_t pointer in as a parameter.
 * @return success / failure of initializing.
 */
ret_t rpt_init(ret_t (*putchr)(uint8_t), ret_t (*getchr)(uint8_t *))
{
    rpt.putchr = putchr;
    rpt.getchr = getchr;
    rpt.invalid_chrs = 0;
    rpt.timeouts = 0;
    rpt.state = kRpt_ReadMagic_0;

    return RET_OK;
}

/*! Checks if a currently active report has timed out (`RPT_TIMEOUT` ms after the
 *      first magic byte)
 *
 *  @Note: This function is inlined as it is a very small stub of code.
 */
static inline void rpt_checkForTimeout(void) {
    if ( HAL_GetTick() > rpt.start_time + RPT_TIMEOUT ) {
        rpt.timeouts += 1;
        rpt.state = kRpt_ReadMagic_0;
    }
}


void rpt_run(void)
{
    uint8_t chr;
    ret_t retval = RET_GEN_ERR;

    // report buffers and variables
    static uint8_t rpt_type = 0;
    static uint8_t rpt_in_buff_len = 0;

    // Keeps track of how many bytes of the payload we've read in
    static uint8_t payload_readin_ind = 0;

    switch (rpt.state) {
        case kRpt_ReadMagic_0:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                if (chr == RPT_MAGIC_NUMBER_0) {
                    rpt.read_buff[0] = chr;
                    rpt.start_time = HAL_GetTick();
                    rpt.state = kRpt_ReadMagic_1;
                } else {
                    // invalid character!
                    rpt.invalid_chrs += 1;
                }
            }
            break;

        case kRpt_ReadMagic_1:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                if (chr == RPT_MAGIC_NUMBER_1) {
                    rpt.read_buff[1] = chr;
                    rpt.state = kRpt_ReadMagic_2;
                } else {
                    // invalid character!
                    rpt.invalid_chrs += 1;
                }
            }

            // Check for timeout
            rpt_checkForTimeout();
            break;

        case kRpt_ReadMagic_2:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                if (chr == RPT_MAGIC_NUMBER_2) {
                    rpt.read_buff[2] = chr;
                    rpt.state = kRpt_ReadMagic_3;
                } else {
                    // invalid character!
                    rpt.invalid_chrs += 1;
                }
            }

            // Check for timeout
            rpt_checkForTimeout();
            break;

        case kRpt_ReadMagic_3:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                if (chr == RPT_MAGIC_NUMBER_3) {
                    rpt.read_buff[3] = chr;
                    rpt.state = kRpt_ReadRpt;
                } else {
                    // invalid character!
                    rpt.invalid_chrs += 1;
                }
            }

            // Check for timeout
            rpt_checkForTimeout();
            break;

        case kRpt_ReadRpt:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                // 0-255 value is valid.
                rpt_type = chr;
                rpt.read_buff[4] = chr;
                rpt.state = kRpt_ReadLen;
            }

            // Check for timeout
            rpt_checkForTimeout();
            break;

        case kRpt_ReadLen:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                rpt_in_buff_len = chr;
                rpt.read_buff[5] = chr;
                payload_readin_ind = RPT_HEADER_SIZE;  // Account for the header data

                if (rpt_in_buff_len != 0) {
                    // This report does have a payload. Read it in.
                    rpt.state = kRpt_ReadPayload;
                } else {
                    // nothing to read in, skip to execution
                    rpt.state = kRpt_Checksum;
                }
            }

            // Check for timeout
            rpt_checkForTimeout();
            break;

        case kRpt_ReadPayload:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                rpt.read_buff[payload_readin_ind] = chr;
                payload_readin_ind += 1;

                if (payload_readin_ind == rpt_in_buff_len + RPT_HEADER_SIZE) {
                    rpt.state = kRpt_Checksum;
                    break;
                }
            }

            // Check for timeout
            rpt_checkForTimeout();
            break;

        case kRpt_Checksum:
            retval = UART_getChar(&chr);
            if (retval == RET_OK) {
                rpt_out_sent_ind = 0;
                rpt.read_buff[payload_readin_ind] = chr;
                uint8_t checksum = priv_calcChecksum(
                    rpt.read_buff, payload_readin_ind + 1);
                if (checksum) {
                    // print out the results (checksum error)
                    output_buffer[0] = RPT_MAGIC_NUMBER_0;
                    output_buffer[1] = RPT_MAGIC_NUMBER_1;
                    output_buffer[2] = RPT_MAGIC_NUMBER_2;
                    output_buffer[3] = RPT_MAGIC_NUMBER_3;
                    output_buffer[4] = (rpt_type | RPT_REPORT_MASK);
                    output_buffer[5] = RET_BAD_CHECKSUM;
                    output_buffer[6] = 1;
                    output_buffer[7] = checksum;

                    // add checksum of this transaction
                    checksum = priv_calcChecksum(output_buffer, 8);
                    output_buffer[8] = checksum;

                    rpt_out_buff_len = 9;
                    rpt_out_sent_ind = 9;

                    // Send the control report off
                    //   TODO: check if this fails
                    UART_sendData(output_buffer, 9);
                    rpt.state = kRpt_ReadMagic_0;
                }
                else {
                    // No checksum error! let's finish out the report
                    rpt.state = kRpt_Evaluate;
                    payload_readin_ind = 0;
                }
            }

            // Check for timeout
            rpt_checkForTimeout();
            break;

        case kRpt_Evaluate:
            // Fetch and execute the requested function
            retval = priv_reportLookup(
                rpt_type, rpt.read_buff, rpt_in_buff_len,
                output_buffer + 7, &rpt_out_buff_len);

            // print out the results
            output_buffer[0] = RPT_MAGIC_NUMBER_0;
            output_buffer[1] = RPT_MAGIC_NUMBER_1;
            output_buffer[2] = RPT_MAGIC_NUMBER_2;
            output_buffer[3] = RPT_MAGIC_NUMBER_3;
            output_buffer[4] = rpt_type | RPT_REPORT_MASK;
            output_buffer[5] = retval;
            output_buffer[6] = rpt_out_buff_len;

            rpt_out_buff_len += 7;
            rpt_out_sent_ind = 7;

            // Add checksum to the end of the reply output_buffer
            uint8_t checksum = priv_calcChecksum(
                output_buffer, rpt_out_buff_len);
            output_buffer[rpt_out_buff_len] = checksum;
            rpt_out_buff_len += 1;  // account for the checksum

            // Send the initial reply out, non-blocking
            while (UART_TXisReady() == false);  // wait for UART to be ready if it isn't
            UART_sendData(output_buffer, rpt_out_buff_len);
            // finish the rest in the next stage
            rpt.state = kRpt_ReadMagic_0;
            break;

        default:
            // Should never get here...
            fatal_error_handler(__FILE__, __LINE__, rpt.state);
            break;
    }
}

/*! Function to send a "stream" report
 *
 */
ret_t rpt_sendStreamReport(uint8_t reportID, uint8_t payload_len, uint8_t * payload_ptr)
{
    // Build up the report and send it off
    output_buffer[0] = RPT_MAGIC_NUMBER_0;
    output_buffer[1] = RPT_MAGIC_NUMBER_1;
    output_buffer[2] = RPT_MAGIC_NUMBER_2;
    output_buffer[3] = RPT_MAGIC_NUMBER_3;
    output_buffer[4] = reportID | RPT_STREAM_MASK;
    output_buffer[5] = 0;  // currently no sense of retval in stream mode
    output_buffer[6] = payload_len;

    // Start the input report to be handled by the runner
    rpt_out_sent_ind = 0;
    rpt_out_buff_len = payload_len + 8;
    memcpy(output_buffer + 7, payload_ptr, payload_len);
    output_buffer[payload_len + 7] = priv_calcChecksum(output_buffer, payload_len + 7);

    // Send it offff
    UART_sendData(output_buffer, payload_len + 8);
    return RET_OK;
}


/* ---------------- Define all reports that can be get / set ---------------- */

/* ------ REPORT REPORTS ------ */
// # Meta

/*! Default report when we don't have the requested report defined.
 */
ret_t rpt_err(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
              uint8_t * UNUSED_PARAM(out_p), uint8_t * UNUSED_PARAM(out_len_ptr))
{
    return RET_NORPT_ERR;
}

/*! Report 0x10 that gets the number of times a report has timed out.
 */
ret_t rpt_report_getTimeoutCount(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                 uint8_t * out_p, uint8_t * out_len_ptr)
{
    // We don't need input data
    *out_len_ptr = sizeof(rpt.timeouts);
    *(uint32_t *)out_p = rpt.timeouts;
    return RET_OK;
    // return in_len;
}

/*! Report 0x11 that gets the number of invalid characters received by this module.
 */
ret_t rpt_report_getInvalidCharsCount(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                      uint8_t * out_p, uint8_t * out_len_ptr)
{
    // We don't need input data
    *out_len_ptr = sizeof(rpt.invalid_chrs);
    *(uint32_t *)out_p = rpt.invalid_chrs;
    return RET_OK;
}


/* ---------- PENSEL REPORTS ---------- */

// These are reports relating to pensel internals

/*! Report 0x30 returns the pensels major and minor version
 */
ret_t rpt_pensel_getVersion(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                            uint8_t * out_p, uint8_t * out_len_ptr)
{
    *out_len_ptr = 6;
    out_p[0] = (uint8_t)PENSEL_VERSION_MAJOR;
    out_p[1] = (uint8_t)PENSEL_VERSION_MINOR;
    *(uint32_t *)(out_p + 3) = (uint32_t)PENSEL_VERSION_GITTAG;
    return RET_OK;
}

/*! Report 0x31 returns the pensels current system time (in ms)
 */
ret_t rpt_pensel_getTimestamp(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                              uint8_t * out_p, uint8_t * out_len_ptr)
{
    *(uint32_t *)out_p = HAL_GetTick();
    *out_len_ptr = sizeof(uint32_t);
    return RET_OK;
}

/*! Report 0x32 Returns the pensel's coms errors (currently only UART dropped packets)
 */
ret_t rpt_pensel_getComsErrors(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                              uint8_t * out_p, uint8_t * out_len_ptr)
{
    uint8_t dropped_packets = UART_droppedPackets();
    out_p[0] = dropped_packets;
    *out_len_ptr = 1;
    return RET_OK;
}

/*! Report 0x33 Returns the pensel's button & switch states
 */
ret_t rpt_pensel_getButtonSwitchState(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                      uint8_t * out_p, uint8_t * out_len_ptr)
{
    *(switch_state_t *)out_p = switch_getval();
    *(out_p + 1) = mainbutton_getval();
    *(out_p + 2) = auxbutton_getval();
    *out_len_ptr = 3;
    return RET_OK;
}

/*! Report 0x34 returns the pensel's critical errors
 */
ret_t rpt_pensel_getCriticalErrors(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                                   uint8_t * out_p, uint8_t * out_len_ptr)
{
    *(critical_errors_t *)out_p = gCriticalErrors;
    *out_len_ptr = sizeof(critical_errors_t);
    return RET_OK;
}


/* ------------------------- MASTER LOOKUP FUNCTION ------------------------- */


static ret_t priv_reportLookup(uint8_t rpt_type, uint8_t *input_buff_ptr, uint8_t input_buff_len,
                  uint8_t * output_buffer_ptr, uint8_t * output_buff_len_ptr)
{
    // define the report lookup table
    static ret_t (*report_function_lookup[])(uint8_t *input_buff_ptr, uint8_t input_buff_len,
                                             uint8_t * output_buff_ptr,
                                             uint8_t * output_buff_len_ptr) = {
        /* Report 0x00 */ rpt_err,
        /* Report 0x01 */ rpt_err,
        /* Report 0x02 */ rpt_err,
        /* Report 0x03 */ rpt_err,
        /* Report 0x04 */ rpt_err,
        /* Report 0x05 */ rpt_err,
        /* Report 0x06 */ rpt_err,
        /* Report 0x07 */ rpt_err,
        /* Report 0x08 */ rpt_err,
        /* Report 0x09 */ rpt_err,
        /* Report 0x0a */ rpt_err,
        /* Report 0x0b */ rpt_err,
        /* Report 0x0c */ rpt_err,
        /* Report 0x0d */ rpt_err,
        /* Report 0x0e */ rpt_err,
        /* Report 0x0f */ rpt_err,
        /* Report 0x10 */ rpt_report_getTimeoutCount,
        /* Report 0x11 */ rpt_report_getInvalidCharsCount,
        /* Report 0x12 */ rpt_err,
        /* Report 0x13 */ rpt_err,
        /* Report 0x14 */ rpt_err,
        /* Report 0x15 */ rpt_err,
        /* Report 0x16 */ rpt_err,
        /* Report 0x17 */ rpt_err,
        /* Report 0x18 */ rpt_err,
        /* Report 0x19 */ rpt_err,
        /* Report 0x1a */ rpt_err,
        /* Report 0x1b */ rpt_err,
        /* Report 0x1c */ rpt_err,
        /* Report 0x1d */ rpt_err,
        /* Report 0x1e */ rpt_err,
        /* Report 0x1f */ rpt_err,
        /* Report 0x20 */ rpt_LSM303DLHC_enableStreams,
        /* Report 0x21 */ rpt_LSM303DLHC_changeConfig,
        /* Report 0x22 */ rpt_LSM303DLHC_getAccel,
        /* Report 0x23 */ rpt_LSM303DLHC_getMag,
        /* Report 0x24 */ rpt_LSM303DLHC_getTemp,
        /* Report 0x25 */ rpt_LSM303DLHC_getErrors,
        /* Report 0x26 */ rpt_LSM303DLHC_setGainAdjustFlag,
        /* Report 0x27 */ rpt_err,
        /* Report 0x28 */ rpt_orient_getPenselOrientation,
        /* Report 0x29 */ rpt_orient_getMagOrientation,
        /* Report 0x2a */ rpt_orient_getAccelOrientation,
        /* Report 0x2b */ rpt_err,
        /* Report 0x2c */ rpt_err,
        /* Report 0x2d */ rpt_err,
        /* Report 0x2e */ rpt_err,
        /* Report 0x2f */ rpt_err,
        /* Report 0x30 */ rpt_pensel_getVersion,
        /* Report 0x31 */ rpt_pensel_getTimestamp,
        /* Report 0x32 */ rpt_pensel_getComsErrors,
        /* Report 0x33 */ rpt_pensel_getButtonSwitchState,
        /* Report 0x34 */ rpt_pensel_getCriticalErrors,
        /* Report 0x35 */ rpt_err,
        /* Report 0x36 */ rpt_err,
        /* Report 0x37 */ rpt_err,
        /* Report 0x38 */ rpt_err,
        /* Report 0x39 */ rpt_err,
        /* Report 0x3a */ rpt_err,
        /* Report 0x3b */ rpt_err,
        /* Report 0x3c */ rpt_err,
        /* Report 0x3d */ rpt_err,
        /* Report 0x3e */ rpt_err,
        /* Report 0x3f */ rpt_err,
        /* Report 0x40 */ rpt_cal_read,
        /* Report 0x41 */ rpt_cal_write,
        /* Report 0x42 */ rpt_err,
        /* Report 0x43 */ rpt_err,
        /* Report 0x44 */ rpt_err,
        /* Report 0x45 */ rpt_err,
        /* Report 0x46 */ rpt_err,
        /* Report 0x47 */ rpt_err,
        /* Report 0x48 */ rpt_err,
        /* Report 0x49 */ rpt_err,
        /* Report 0x4a */ rpt_err,
        /* Report 0x4b */ rpt_err,
        /* Report 0x4c */ rpt_err,
        /* Report 0x4d */ rpt_err,
        /* Report 0x4e */ rpt_err,
        /* Report 0x4f */ rpt_err,
        /* Report 0x50 */ rpt_err,
        /* Report 0x51 */ rpt_err,
        /* Report 0x52 */ rpt_err,
        /* Report 0x53 */ rpt_err,
        /* Report 0x54 */ rpt_err,
        /* Report 0x55 */ rpt_err,
        /* Report 0x56 */ rpt_err,
        /* Report 0x57 */ rpt_err,
        /* Report 0x58 */ rpt_err,
        /* Report 0x59 */ rpt_err,
        /* Report 0x5a */ rpt_err,
        /* Report 0x5b */ rpt_err,
        /* Report 0x5c */ rpt_err,
        /* Report 0x5d */ rpt_err,
        /* Report 0x5e */ rpt_err,
        /* Report 0x5f */ rpt_err,
        /* Report 0x60 */ rpt_err,
        /* Report 0x61 */ rpt_err,
        /* Report 0x62 */ rpt_err,
        /* Report 0x63 */ rpt_err,
        /* Report 0x64 */ rpt_err,
        /* Report 0x65 */ rpt_err,
        /* Report 0x66 */ rpt_err,
        /* Report 0x67 */ rpt_err,
        /* Report 0x68 */ rpt_err,
        /* Report 0x69 */ rpt_err,
        /* Report 0x6a */ rpt_err,
        /* Report 0x6b */ rpt_err,
        /* Report 0x6c */ rpt_err,
        /* Report 0x6d */ rpt_err,
        /* Report 0x6e */ rpt_err,
        /* Report 0x6f */ rpt_err,
        /* Report 0x70 */ rpt_err,
        /* Report 0x71 */ rpt_err,
        /* Report 0x72 */ rpt_err,
        /* Report 0x73 */ rpt_err,
        /* Report 0x74 */ rpt_err,
        /* Report 0x75 */ rpt_err,
        /* Report 0x76 */ rpt_err,
        /* Report 0x77 */ rpt_err,
        /* Report 0x78 */ rpt_err,
        /* Report 0x79 */ rpt_err,
        /* Report 0x7a */ rpt_err,
        /* Report 0x7b */ rpt_err,
        /* Report 0x7c */ rpt_err,
        /* Report 0x7d */ rpt_err,
        /* Report 0x7e */ rpt_err,
        /* Report 0x7f */ rpt_err};

    // call the function specified
    *output_buff_len_ptr = 0;
    if (rpt_type > MAX_REPORT_ID) {
        return RET_NORPT_ERR;
    }

    return report_function_lookup[rpt_type](
        input_buff_ptr, input_buff_len, output_buffer_ptr, output_buff_len_ptr);
}