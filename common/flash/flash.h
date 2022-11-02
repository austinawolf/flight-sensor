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


#define FLASH_SECTOR_SIZE     (4096u)

/**
 * @brief Possible events generated by the flash driver
 */
typedef enum
{
    FLASH_EVENT_WRITE_DONE,
    FLASH_EVENT_READ_DONE,
    FLASH_EVENT_ERASE_DONE,
    FLASH_EVENT_MAX_VALUE,
} flash_event_e;

/**
 * @brief Definition of flash driver event callback function
 */
typedef void (*flash_event_callback_t)(flash_event_e event, void *context);

/**
 * @brief Types of erase operations
 */
typedef enum
{
    FLASH_ERASE_SECTOR,
    FLASH_ERASE_BLOCK,
    FLASH_ERASE_ALL,
} flash_erase_e;

/**
 * @brief 
 */
typedef struct
{
    uint32_t start; ///< first block of allocated flash space
    uint32_t len;   ///< number of blocks allocation
    flash_event_callback_t event_callback;  ///< callback to execute on event
} flash_instance_t;

/**
 * @brief Initializes the flash driver
 * 
 * @return STATUS_OK on success
 */
status_e flash_create(void);

/**
 * @brief Reads from flash device
 * 
 * @param   address offset to read from, in bytes
 * @param   data pointer to read buffer
 * @param   len amount of data to read, in bytes
 * @return STATUS_OK on success
 */
status_e flash_read(flash_instance_t *instance, uint32_t sector, uint32_t offset, uint8_t *data, uint32_t len);

/**
 * Writes to flash device
 *
 * @param address offset to write to, in bytes
 * @param data pointer to data buffer to write
 * @param len  amount of data to write, in bytes
 * @return STATUS_OK on success
 */
status_e flash_write(flash_instance_t *instance, uint32_t sector, uint32_t offset, const uint8_t *data, uint32_t len);

/**
 * Erase section of flash memory
 *
 * @param address address to erase
 * @param len number of bytes to erase
 * @return STATUS_OK on success
 */
status_e flash_erase(flash_instance_t *instance, uint32_t sector, flash_erase_e type);

/**
 * @brief Checks if flash driver is currently reading/writing/erasing
 * 
 * @param is_busy true if busy, false otherwise
 * @return STATUS_OK on success
 */
status_e flash_is_busy(bool *is_busy);

#endif
