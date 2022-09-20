/**
 * @file    flash.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"

typedef enum
{
    FLASH_EVENT_WRITE_DONE,
    FLASH_EVENT_READ_DONE,
    FLASH_EVENT_ERASE_DONE,
    FLASH_EVENT_MAX_VALUE,
} flash_event_e;


typedef void (*flash_event_callback_t)(flash_event_e event, void *context);


/**
 * @brief 
 * 
 */
typedef enum
{
    FLASH_ERASE_SECTOR,
    FLASH_ERASE_BLOCK,
    FLASH_ERASE_ALL,
} flash_erase_e;

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e flash_create(void);

/**
 * Reads from flash device
 * 
 * @param   address     offset to read from, in bytes
 * @param   data        pointer to read buffer
 * @param   len         amount of data to read, in bytes
 *
 * @return  Status_OK on success
 */
status_e flash_read(uint32_t address, uint8_t *data, uint32_t len);

/**
 * Writes to flash device
 *
 * @param   address     offset to write to, in bytes
 * @param   data        pointer to data buffer to write
 * @param   len         amount of data to write, in bytes
 *
 * @return  Status_OK on success
 */
status_e flash_write(uint32_t address, const uint8_t *data, uint32_t len);

/**
 * Erase section of flash memory
 * @param   address     address to erase
 * @param   len         number of bytes to erase
 *
 * @return  Status_OK on success
 */
status_e flash_erase(uint32_t address, flash_erase_e type);

/**
 * @brief 
 * 
 * @param is_busy 
 * @return status_e 
 */
status_e flash_is_busy(bool *is_busy);

/**
 * @brief 
 * 
 */
status_e flash_register_event_handler(flash_event_callback_t callback, void *context);

#endif
