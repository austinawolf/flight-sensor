/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include <string.h>
#include "sample_store.h"
#include "flash.h"


#define SECTOR_INDEX_TO_ADDRESS(__index)     (__index * 4096)


static void _on_write_complate(flash_event_e event, void *context)
{
    if (event != FLASH_EVENT_DONE)
    {
        return;
    }

    sample_store_t *instance = (sample_store_t*) context;
    instance->sector_index++;
    instance->buffer_index = 0u;

    uint32_t address = SECTOR_INDEX_TO_ADDRESS(instance->sector_index);
    (void) flash_erase(address, FLASH_ERASE_SECTOR, NULL, NULL);
}


/**
 * @see sample_store.h
 */
status_e sample_store_create(sample_store_t *instance)
{
    instance->sector_index = 0u;
    instance->buffer_index = 0u;
    
    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e sample_store_reset(sample_store_t *instance)
{
    instance->sector_index = 0u;
    instance->buffer_index = 0u;
    uint32_t address = SECTOR_INDEX_TO_ADDRESS(instance->sector_index);
    status_e status = flash_erase(address, FLASH_ERASE_SECTOR, NULL, NULL);
    APP_ERROR_CHECK(status);

    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e sample_store_append(sample_store_t *instance, const void *data, uint32_t len)
{
    // write to buffer
    (void) memcpy(&instance->write_buffer[instance->buffer_index], data, instance->sample_size);
    instance->buffer_index += instance->sample_size;

    // check if buffer full
    if ((instance->buffer_index + instance->sample_size) < sizeof(instance->write_buffer))
    {
        return STATUS_OK;
    }
    
    // check if busy
    bool is_busy = true;
    (void) flash_is_busy(&is_busy);
    if (is_busy)
    {
        return STATUS_ERROR_INVALID_STATE;
    }

    // start write
    uint32_t address = SECTOR_INDEX_TO_ADDRESS(instance->sector_index);
    status_e status = flash_write(address, (void*) &instance->write_buffer, 4096, _on_write_complate, NULL);
    APP_ERROR_CHECK(status);

    return STATUS_OK;
}

/**
 * @see sample_store.h
 */
status_e sample_store_read(sample_store_t *instance, uint32_t index, void *data, uint32_t len, uint32_t count)
{
    status_e status = flash_read(index * len, data, len * count);
    APP_ERROR_CHECK(status);

    return STATUS_OK;
}