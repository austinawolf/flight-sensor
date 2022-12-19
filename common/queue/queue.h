/**
 * @file    queue.h
 * @author  Austin Wolf
 * @brief   basic fixed size queue
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "status.h"


/**
 * Structure definition of the queue object
 */
typedef struct
{
    void* data_array;           /**< Static storage for the queue data */
    uint32_t array_length;      /**< Length of the data_array */
    uint32_t data_size;         /**< The size of a data entry in the array */
    uint32_t head;              /**< Head of the queue */
    uint32_t num_entries;       /**< Current number of entries in the queue */
} queue_t;

/**
 * Creates a queue with the parameters given. It utilizes static storage by having the caller
 * pass in the array to store the data in
 * @note this function also takes care of semaphore and mutex initialization (if using them)
 *
 * @param   queue         the queue to create
 * @param   array         pointer to the data array to store the queue data
 * @param   array_length  the length of the array
 * @param   data_size     the size of one entry in the array
 *
 * @return  STATUS_OK if successful, #queue_status_e otherwise
 */
status_e queue_create(queue_t* queue, void* array, uint32_t array_length, uint32_t data_size);

/**
 * Adds the data given to the end of the given queue
 *
 * @param   queue       the queue to append the data to
 * @param   data        the data to add to the queue.  It is assumed that the data size matches the data size passed in on create
 *
 * @return  STATUS_OK if successful, #queue_status_e otherwise
 */
status_e queue_append(queue_t* queue, const void* data);

/**
 * Pops the item from the queue at the given offset from the head of the queue
 *
 * @param   queue       the queue to get the next data from
 * @param   offset      the offset from the head of the queue to pop out. A value of 0 will pop the head of the queue
 * @param   data        pointer to store the data.  It is assumed that the data size matches the data size passed in on create
 *
 * @return  STATUS_OK if successful, #queue_status_e otherwise
 */
status_e queue_pop(queue_t* queue, uint32_t offset, void* data);

/**
 * Gets the entry at the location in the given queue without removing the item from the queue.  If the queue is empty, will return #QUEUE_EMPTY
 *
 * @param   queue   the queue to get the next data from
 * @param   offset  the offset to get from the beginning of queue. A value of 0 will inspect the current head of the queue
 * @param   data    pointer to store the data.  It is assumed that the data size matches the data size passed in on create
 *
 * @return  STATUS_OK if successful, #queue_status_e otherwise
 */
status_e queue_inspect(const queue_t* queue, uint32_t offset, void* data);

/**
 * Flushes the given queue
 * @note any semaphores associated with the queue will be destroyed and re-created
 *
 * @param   queue  the queue to flush
 *
 * @return  STATUS_OK if successful, #queue_status_e otherwise
 */
status_e queue_flush(queue_t* queue);

/**
 * Gets the number of entries currently in the queue
 *
 * @param   queue  the queue to check
 *
 * @return  the number of entries in the queue
 */
uint32_t queue_number_of_entries(const queue_t* queue);

/**
 * Checks if a given queue is empty
 *
 * @param   queue  the queue to check
 *
 * @return  TRUE if queue is empty, FALSE otherwise
 */
bool queue_is_empty(const queue_t* queue);

#endif /* _QUEUE_API_H_ */
