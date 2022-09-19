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
#define SECTOR_INDEX_TO_ADDRESS(__index)     (__index * SECTOR_SIZE)
#define ONE_MB              (1024 * 1024)
#define SAMPLES_PER_SECTOR  (104)

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
    bool write_in_progress;
    bool erase_in_progress;
    const uint32_t start;
    const uint32_t len;
    volatile uint16_t sector_index;
} sample_store_control_t;


static sample_store_control_t _control = {0};

static void _on_erase_complate(flash_event_e event, void *context)
{
    if (event != FLASH_EVENT_DONE)
    {
        return;
    }

    _control.erase_in_progress = false;
}

static void _on_write_complate(flash_event_e event, void *context)
{
    if (event != FLASH_EVENT_DONE)
    {
        return;
    }

    _control.write_in_progress = false;
    _control.sector_index++;

    uint32_t address = SECTOR_INDEX_TO_ADDRESS(_control.sector_index);
    (void) flash_erase(address, FLASH_ERASE_SECTOR, _on_erase_complate, NULL);
    _control.erase_in_progress = true;
}




/**
 * @see sample_store.h
 */
status_e session_store_create(void)
{
    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e session_store_clear(void)
{
    _control.sector_index = 0u;
    uint32_t address = SECTOR_INDEX_TO_ADDRESS(_control.sector_index);
    status_e status = flash_erase(address, FLASH_ERASE_SECTOR, NULL, NULL);
    if (status != STATUS_OK)
    {
        return STATUS_ERROR;
    }

    _control.erase_in_progress = true;

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
    _write_to_buffer(&_control.buffers[_control.current_buffer], data);

    // check if buffer full
    if (_control.buffers[_control.current_buffer].header.count < SAMPLES_PER_SECTOR)
    {
        return STATUS_OK;
    }

    // check if last write/erase finished
    if (_control.write_in_progress || _control.erase_in_progress)
    {
        LOG_ERROR("Last write/erase didn't finish (WIP=%d, EIP=%d)", _control.write_in_progress, _control.erase_in_progress);
        return STATUS_ERROR;
    }

    // switch buffers
    uint8_t buffer_to_write = _control.current_buffer;
    _control.current_buffer = OTHER_BUFFER(_control.current_buffer);
    memset(&_control.current_buffer, 0, sizeof(sample_store_block_t));

    // start write
    uint32_t address = SECTOR_INDEX_TO_ADDRESS(_control.sector_index);
    status_e status = flash_write(address, (uint8_t*) &_control.buffers[buffer_to_write], sizeof(sample_store_block_t), _on_write_complate, NULL);
    if (status != STATUS_OK)
    {
        LOG_ERROR("flash_write failed, err: %d", status);
    }
    _control.write_in_progress = true;

    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e session_store_read(uint32_t index, imu_sample_t *data)
{

    return STATUS_OK;
}