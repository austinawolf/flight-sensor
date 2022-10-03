/**
 * @file    session_store.c
 * @author  Austin Wolf
 * @brief   Double buffered data store of sample data saved in flash
 */

#include <string.h>
#include "session_store.h"
#include "flash.h"
#include "logger.h"


#define SECTOR_SIZE         (4096u)         ///< Size of single sector, in bytes
#define ONE_MB              (1024 * 1024)   ///< Representation of 1 megabyte
#define SAMPLES_PER_SECTOR  (92)            ///< Samples per sector
#define SECTOR_PREAMBLE     (0xBEEF)        ///< 32-bit sector preamble

/**
 * Helper macro to get address from start index and sector index
 */
#define SECTOR_INDEX_TO_ADDRESS(__start, __index)     ((__start + __index) * SECTOR_SIZE)

/**
 * Helper macro to select the other buffer
 */
#define OTHER_BUFFER(_current)        ((_current + 1) % 2)

/**
 * Header used to make the beginning of each sector
 */
typedef struct
{
    uint16_t preamble;
    uint16_t count;
} sector_header_t;

/**
 * Buffer used to load samples
 */
typedef struct
{
    sector_header_t header;
    imu_sample_t samples[SAMPLES_PER_SECTOR];
} sector_buffer_t;

/**
 * Definition of fields in control block
 */
typedef struct
{
    uint32_t start;
    uint32_t len;
    sector_buffer_t buffers[2];
    uint8_t current_buffer;
    volatile bool write_in_progress;
    volatile bool erase_in_progress;
    bool is_open;
    volatile uint16_t sector_index;
} session_store_control_t;

/// Control block, initialized on create
static session_store_control_t _control = {0};

/**
 * Erases a single sector at sector index
 */
static status_e _erase_sector(uint32_t sector_index)
{
    uint32_t address = SECTOR_INDEX_TO_ADDRESS(_control.start, sector_index);
    status_e status = flash_erase(address, FLASH_ERASE_SECTOR);
    if (status != STATUS_OK)
    {
        return status;
    }
    _control.erase_in_progress = true;

    return STATUS_OK;
}

/**
 * Writes data to a single sector at sector index
 */
static status_e _write_sector(uint32_t sector_index, uint8_t *data, uint32_t len)
{
    uint32_t address = SECTOR_INDEX_TO_ADDRESS(_control.start, sector_index);

    status_e status = flash_write(address, data, len);
    if (status != STATUS_OK)
    {
        LOG_ERROR("flash_write failed, err: %d", status);
        return status;
    }
    _control.write_in_progress = true;

    return STATUS_OK;
}

/**
 * Readies the other write buffer
 */
static void _swap_buffers(void)
{
    // switch buffers
    _control.current_buffer = OTHER_BUFFER(_control.current_buffer);
    memset(&_control.buffers[_control.current_buffer], 0, sizeof(sector_buffer_t));
    _control.buffers[_control.current_buffer].header.count = 0u;
}

/**
 * Reads a sample from a sector and provided offset
 */
static status_e _read_sample(uint32_t sector_index, uint32_t sector_offset, imu_sample_t *data)
{
    uint32_t header_address = SECTOR_INDEX_TO_ADDRESS(_control.start, sector_index);
    uint32_t sample_address = header_address + sizeof(sector_header_t) + sizeof(imu_sample_t) * sector_offset;

    sector_header_t header = {0};
    status_e status = flash_read(header_address, (uint8_t*) &header, sizeof(header));
    if (status != STATUS_OK)
    {
        return status;
    }

    // block until read is done
    bool is_busy = false;
    (void) flash_is_busy(&is_busy);
    while (is_busy)
    {
        (void) flash_is_busy(&is_busy);
    }
    
    if (header.preamble != SECTOR_PREAMBLE)
    {
        LOG_ERROR("header.preamble=0x%x, header.count=0x%x", header.preamble, header.count);
        return STATUS_ERROR_INVALID_PARAM;
    }

    // check if enough data is stored in this sector
    if (sector_offset > header.count)
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    status = flash_read(sample_address, (uint8_t*) data, sizeof(imu_sample_t));
    if (status != STATUS_OK)
    {
        return status;
    }

    // block until read is done
    is_busy = false;
    (void) flash_is_busy(&is_busy);
    while (is_busy)
    {
        (void) flash_is_busy(&is_busy);
    }

    return STATUS_OK;
}

/**
 * Event handler for events from flash driver
 */
static void _flash_event_handler(flash_event_e event, void *context)
{
    switch (event)
    {
        case FLASH_EVENT_WRITE_DONE:
            _control.write_in_progress = false;
            if (!_control.is_open)
            {
                break;
            }
            status_e status = _erase_sector(_control.sector_index);
            if (status != STATUS_OK)
            {
                LOG_ERROR("_erase_sector failed, err: %d", status);
            }
            break;
        case FLASH_EVENT_ERASE_DONE:
            _control.erase_in_progress = false;
            break;
        case FLASH_EVENT_READ_DONE:
            break;
        default:
            break;
    }
}

/**
 * @see sample_store.h
 */
status_e session_store_create(void)
{
    flash_register_event_handler(_flash_event_handler, NULL);

    _control.start = 10;
    _control.len = 30;

    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e session_store_open(void)
{
    _control.sector_index = 0u;

    // erase the next sector to write
    status_e status = _erase_sector(_control.sector_index);
    if (status != STATUS_OK)
    {
        return status;
    }

    // ready the buffer
    _swap_buffers();

    _control.is_open = true;

    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e session_store_close(void)
{
    // handle special case where all data is already written out
    if (_control.buffers[_control.current_buffer].header.count == 0)
    {
        _control.is_open = false;
        return STATUS_OK;
    }

    // check if last write/erase finished
    if (_control.write_in_progress || _control.erase_in_progress)
    {
        LOG_ERROR("Last write/erase didn't finish (WIP=%d, EIP=%d)", _control.write_in_progress, _control.erase_in_progress);
        return STATUS_ERROR_INVALID_STATE;
    }

    // write buffer to sector
    sector_buffer_t *buffer = &_control.buffers[_control.current_buffer];
    buffer->header.preamble = SECTOR_PREAMBLE;
    uint32_t len = sizeof(sector_header_t) + sizeof(imu_sample_t) * buffer->header.count; 
    status_e status = _write_sector(_control.sector_index, (uint8_t*) buffer, len);
    if (status != STATUS_OK)
    {
        return status;
    }

    LOG_INFO("Stored %d samples", _control.sector_index * SAMPLES_PER_SECTOR + buffer->header.count);
    
    _control.is_open = false;

    return STATUS_OK;
}

/**
 * @brief Writes a sample active to buffer
 */
static void _write_to_buffer(sector_buffer_t *buffer, const imu_sample_t *data)
{
    (void) memcpy(&buffer->samples[buffer->header.count], data, sizeof(imu_sample_t));
    buffer->header.count++;
}

/**
 * @see sample_store.h
 */
status_e session_store_append(const imu_sample_t *data)
{
    // check is store is full
    if (_control.sector_index >= (_control.start + _control.len))
    {
        return STATUS_ERROR_FLASH_FULL;
    }

    // write to buffer
    sector_buffer_t *buffer = &_control.buffers[_control.current_buffer];
    _write_to_buffer(buffer, data);

    // check if buffer full
    if (buffer->header.count < SAMPLES_PER_SECTOR)
    {
        return STATUS_OK;
    }

    // check if last write/erase finished
    if (_control.write_in_progress || _control.erase_in_progress)
    {
        LOG_ERROR("Last write/erase didn't finish (WIP=%d, EIP=%d)", _control.write_in_progress, _control.erase_in_progress);
        return STATUS_ERROR_INVALID_STATE;
    }

    // swap buffers
    _swap_buffers();

    // write buffer to sector
    buffer->header.preamble = SECTOR_PREAMBLE;
    uint32_t len = sizeof(sector_header_t) + sizeof(imu_sample_t) * buffer->header.count; 
    status_e status = _write_sector(_control.sector_index, (uint8_t*) buffer, len);
    if (status != STATUS_OK)
    {
        return status;
    }

    // Incremement sector index to indicate next free sector
    _control.sector_index++;

    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e session_store_read(uint32_t index, imu_sample_t *data)
{
    if (_control.is_open)
    {
        return STATUS_ERROR_INVALID_STATE;
    }

    uint32_t sector_index = index / SAMPLES_PER_SECTOR;
    uint32_t sector_offset = index % SAMPLES_PER_SECTOR;

    // check is sector index is valid
    if (sector_index >= (_control.start + _control.len))
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    return _read_sample(sector_index, sector_offset, data);
}

/**
 * @see sample_store.h
 */
bool session_store_is_busy(void)
{
    return _control.erase_in_progress || _control.write_in_progress;
}
