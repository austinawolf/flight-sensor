/**
 * @file    queue.c
 * @author  Austin Wolf
 * @brief   basic fixed size queue
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "queue.h"


/**
 * Gets the pointer to the queue's data at the offset given
 *
 * @param   queue   the queue to get the data location for
 * @param   offset  the offset from the beginning of the queue
 *
 * @return  Pointer to where the data is stored for that entry
 */
static inline void* _queue_get_data_location_at_offset(const queue_t* queue, uint32_t offset)
{
    uint32_t i = ((queue->head - queue->num_entries) + offset + queue->array_length) % queue->array_length;

    return (queue->data_array + (i * queue->data_size)); /*lint !e124 !e9016 pointer arithmetic accounts for element size */
}

/**
 * @see queue_api.h
 */
status_e queue_create(queue_t* queue, void* array, uint32_t array_length, uint32_t data_size)
{
    if (NULL == queue || NULL == array)
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    if ((array_length == 0u) || (data_size == 0u))
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    queue->data_array = array;
    queue->data_size = data_size;
    queue->array_length = array_length;
    queue->head = 0;
    queue->num_entries = 0;

    return STATUS_OK;
}

/**
 * @see queue_api.h
 */
status_e queue_append(queue_t* queue, const void* data)
{
    if (NULL == queue || NULL == data)
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    if (queue->num_entries == queue->array_length)
    {
        status_e status = queue_pop(queue, 0, NULL);
        if (STATUS_OK != status)
        {
            return status;
        }
    }

    void* data_loc = queue->data_array + (queue->head * queue->data_size); /*lint !e124 !e9016 pointer arithmetic accounts for element size */
    memcpy(data_loc, data, queue->data_size);
    queue->head = (queue->head + 1u) % queue->array_length;
    queue->num_entries++;

    return STATUS_OK;
}


/**
 * @see queue_api.h
 */
status_e queue_pop(queue_t* queue, uint32_t offset, void* data)
{
    if (NULL == queue)
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    // Check if queue is empty again inside mutex. If so, unlock mutex and exit
    if (queue->num_entries == 0u)
    {
        return STATUS_ERROR_EMPTY;
    }

    // Check that the offset is less than the current number of entries
    if (offset >= queue->num_entries)
    {
        return STATUS_ERROR;
    }

    const void* data_loc = _queue_get_data_location_at_offset(queue, offset);

    if (data != NULL)
    {
        memcpy(data, data_loc, queue->data_size);
    }

    // Shift the beginning of the queue towards the head to remove this entry
    for (int32_t i = (int32_t) offset - 1; i >= 0; i--)
    {
        const void* old_data_loc = _queue_get_data_location_at_offset(queue, (uint32_t) i);
        void* new_data_loc = _queue_get_data_location_at_offset(queue, (uint32_t) i + 1u);
        memmove(new_data_loc, old_data_loc, queue->data_size);
    }

    queue->num_entries--;

    return STATUS_OK;
}


/**
 * @see queue_api.h
 */
status_e queue_inspect(const queue_t* queue, uint32_t offset, void* data)
{
    if (NULL == queue || NULL == data)
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    // Check if queue is empty again inside mutex. If so, unlock mutex and exit
    if (queue->num_entries == 0u)
    {
        return STATUS_ERROR_EMPTY;
    }

    // Check that the offset is less than the current number of entries
    if (offset >= queue->num_entries)
    {
        return STATUS_ERROR;
    }

    // Get the offset index given the number of entries and current head location
    const void* data_loc = _queue_get_data_location_at_offset(queue, offset);
    memcpy(data, data_loc, queue->data_size);

    return STATUS_OK;
}

/**
 * @see queue_api.h
 */
status_e queue_flush(queue_t* queue)
{
    if (NULL == queue)
    {
        return STATUS_ERROR_INVALID_PARAM;
    }

    queue->num_entries = 0;
    queue->head = 0;

    return STATUS_OK;
}

/**
 * @see queue_api.h
 */
uint32_t queue_number_of_entries(const queue_t* queue)
{
    if (NULL == queue)
    {
        return 0u;
    }

    return queue->num_entries;
}

/**
 * @see queue_api.h
 */
bool queue_is_empty(const queue_t* queue)
{
    return (queue_number_of_entries(queue) == 0u);
}
