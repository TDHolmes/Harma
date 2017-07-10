/*!
 * @file    cal.c
 * @author  Tyler Holmes
 * @version 0.1.0
 * @date    20-May-2017
 * @brief   Calibration code and structures
 */

#include <stdint.h>
#include <string.h>

#include "stm32f3xx.h"
#include "Drivers/stm32f3xx_hal_flash.h"
#include "Drivers/stm32f3xx_hal_flash_ex.h"
#include "common.h"
#include "cal.h"
#include "default_cal.h"

// Private function definitions
uint8_t cal_calcChecksum(uint32_t length, uint8_t * data_ptr);
ret_t cal_writeToFlash(uint32_t * dataToWrite_ptr);


/*! Function to calculate the checksum of a block of memory. Simple twos complement addition
 *
 * @param length (uint32_t): Number of bytes in the data to be calculated.
 * @param data_ptr (uint8_t *): Pointer to the data in memory.
 * @return checksum (uint8_t): The calculated checksum.
 */
uint8_t cal_calcChecksum(uint32_t length, uint8_t * data_ptr)
{
    uint8_t checksum = 0;

    while(length > 0) {
        checksum += *data_ptr;
        data_ptr++;
        length--;
    }
    return checksum;
}

/*! Function to load calibration from flash memory
 *
 * @param length (uint32_t): Number of bytes in the data to be calculated.
 * @param data_ptr (uint8_t *): Pointer to the data in memory.
 * @return checksum (uint8_t): The calculated checksum.
 */
ret_t cal_loadFromFlash(void)
{
    ret_t retval = RET_OK;
    // Load calibration from flash into gCal
    uint32_t * cal_ptr = (uint32_t *)&gCal;
    uint32_t * address = (uint32_t *)PENSEL_CAL_START_ADDRESS;
    while (address < (uint32_t *)PENSEL_CAL_END_ADDRESS) {
        *cal_ptr = *address;
        address += 4;
        cal_ptr += 4;
    }

    return retval;
}

/*! Function to load sane default calibration in case of corrupt cal
 *
 * @return retval (ret_t): Success or failure
 */
ret_t cal_loadDefaults(void)
{
    gCal = default_cal;
    return RET_OK;
}

/*! Function to check the loaded calibration for validity (version, header, and checksum)
 *
 * @return retval (ret_t): Success or failure
 */
ret_t cal_checkValidity(void)
{
    ret_t retval = RET_OK;
    if (gCal.version != PENSEL_CAL_VERSION) {
        // cal version error!
        gCriticalErrors.cal_version_err = 1;
        retval = RET_CAL_ERR;
    }
    if (gCal.header != PENSEL_CAL_HEADER) {
        // cal header error!
        gCriticalErrors.cal_header_err = 1;
        retval = RET_CAL_ERR;
    }
    if (cal_calcChecksum(sizeof(pensel_cal_t), (uint8_t *)&gCal)) {
        // cal checksum error!
        gCriticalErrors.cal_checksum_err = 1;
        retval = RET_CAL_ERR;
    }
    return retval;
}

/*! Function to write calibration data to flash.
 *
 * @param dataToWrite_ptr (uint32_t *): A pointer to the data to be written to flash
 *      as calibration data.
 * @return retval (ret_t): Success or failure
 *
 * @Note Does not check the values being written for validity.
 */
ret_t cal_writeToFlash(uint32_t * dataToWrite_ptr)
{
    ret_t retval = RET_OK;
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t pageError, address;

    // allow flash to be written to
    HAL_FLASH_Unlock();

    // configure the type of erasing we're going to do
    eraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.PageAddress = PENSEL_CAL_START_ADDRESS;
    eraseInitStruct.NbPages     = 1;  // Calibration better be smaller than 2kB...

    if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK) {
        retval = RET_GEN_ERR;
    } else {

        // Start flashing data
        address = PENSEL_CAL_START_ADDRESS;

        while (address < PENSEL_CAL_END_ADDRESS) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *dataToWrite_ptr) == HAL_OK) {
                address = address + 4;
                dataToWrite_ptr = dataToWrite_ptr + 4;
            } else {
                retval = RET_GEN_ERR;
            }
        }
    }

    // Always lock the flash after
    HAL_FLASH_Lock();
    return retval;
}


// ----------- Report functions -----------
ret_t rpt_cal_read(uint8_t * UNUSED_PARAM(in_p), uint8_t UNUSED_PARAM(in_len),
                   uint8_t * out_p, uint8_t * out_len_ptr)
{
    *out_len_ptr = sizeof(pensel_cal_t);
    *(pensel_cal_t *)out_p = gCal;
    return RET_OK;
}


ret_t rpt_cal_write(uint8_t * in_p, uint8_t in_len, uint8_t * UNUSED_PARAM(out_p), uint8_t * out_len_ptr)
{
    *out_len_ptr = 0;
    if (in_len != sizeof(pensel_cal_t)) {
        return RET_LEN_ERR;
    }
    return cal_writeToFlash((uint32_t *)in_p);
}
