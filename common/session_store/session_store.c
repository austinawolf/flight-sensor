/**
 * @file    session_store.c
 * @author  Austin Wolf
 * @brief
 */
#include <string.h>
#include "session_store.h"
#include "flash.h"
#include "logger.h"


#define SECTOR_SIZE                          (4096u)
#define SECTOR_INDEX_TO_ADDRESS(__start, __index)     ((__start + __index) * SECTOR_SIZE)
#define ONE_MB              (1024 * 1024)
#define SAMPLES_PER_SECTOR  (92)
#define SECTOR_PREAMBLE     (0xDEADBEEF)

#define OTHER_BUFFER(_current)        ((_current + 1) % 2)

typedef struct
{
    uint32_t preamble;
    uint32_t count;
} sample_store_block_header_t;

typedef struct
{
    sample_store_block_header_t header;
    imu_sample_t samples[SAMPLES_PER_SECTOR];
} sample_store_block_t;

typedef struct
{
    sample_store_block_t buffers[2];
    uint8_t current_buffer;
    volatile bool write_in_progress;
    volatile bool erase_in_progress;
    bool is_open;
    uint32_t start;
    uint32_t len;
    volatile uint16_t sector_index;
} sample_store_control_t;


static sample_store_control_t _control = 
{
    .start = 2,
    .len = 20,
};

static status_e _erase_sector(uint32_t sector_index)
{
    uint32_t address = SECTOR_INDEX_TO_ADDRESS(_control.start, sector_index);
    status_e status = flash_erase(address, FLASH_ERASE_SECTOR);
    if (status != STATUS_OK)
    {
        return STATUS_ERROR;
    }
    _control.erase_in_progress = true;

    return STATUS_OK;
}

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

static void _swap_buffers(void)
{
    // switch buffers
    _control.current_buffer = OTHER_BUFFER(_control.current_buffer);
    memset(&_control.buffers[_control.current_buffer], 0, sizeof(sample_store_block_t));
    _control.buffers[_control.current_buffer].header.count = 0u;
}

static void _flash_event_handler(flash_event_e event, void *context)
{
    switch (event)
    {
        case FLASH_EVENT_WRITE_DONE:
            LOG_INFO("Write complete");
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
            LOG_INFO("Erase complete");
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
        return STATUS_ERROR;
    }

    // write buffer to sector
    sample_store_block_t *buffer = &_control.buffers[_control.current_buffer];
    buffer->header.preamble = SECTOR_PREAMBLE;
    uint32_t len = sizeof(sample_store_block_header_t) + sizeof(imu_sample_t) * buffer->header.count; 
    status_e status = _write_sector(_control.sector_index, (uint8_t*) buffer, len);
    if (status != STATUS_OK)
    {
        return status;
    }

    _control.is_open = false;

    return STATUS_OK;
}


static void _write_to_buffer(sample_store_block_t *buffer, const imu_sample_t *data)
{
    (void) memcpy(&buffer->samples[buffer->header.count], data, sizeof(imu_sample_t));
    buffer->header.count++;
}

/**
 * @see sample_store.h
 */
status_e session_store_append(const imu_sample_t *data)
{
    // write to buffer
    sample_store_block_t *buffer = &_control.buffers[_control.current_buffer];
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
        return STATUS_ERROR;
    }

    // swap buffers
    _swap_buffers();

    // write buffer to sector
    buffer->header.preamble = SECTOR_PREAMBLE;
    uint32_t len = sizeof(sample_store_block_header_t) + sizeof(imu_sample_t) * buffer->header.count; 
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
    uint32_t sector_index = index / SAMPLES_PER_SECTOR;
    uint32_t sector_offset = index % SAMPLES_PER_SECTOR;
    uint32_t header_address = SECTOR_INDEX_TO_ADDRESS(_control.start, sector_index);
    uint32_t sample_address = header_address + sizeof(sample_store_block_header_t) + sizeof(imu_sample_t) * sector_offset;

    sample_store_block_header_t header = {0};
    status_e status = flash_read(header_address, (uint8_t*) &header, sizeof(header));
    if (status != STATUS_OK)
    {
        return STATUS_ERROR;
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
        LOG_INFO("header.preamble=0x%x, header.count=0x%x", header.preamble, header.count);
        return STATUS_ERROR;
    }

    if (sector_offset > header.count)
    {
        return STATUS_ERROR;
    }

    status = flash_read(sample_address, (uint8_t*) data, sizeof(imu_sample_t));
    if (status != STATUS_OK)
    {
        return STATUS_ERROR;
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

bool session_store_is_busy(void)
{
    return _control.erase_in_progress || _control.write_in_progress;
}


