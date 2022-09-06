/**
 * @file    sample_store.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef SAMPLE_STORE_H_
#define SAMPLE_STORE_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"



#define ONE_MB  (1024 * 1024)

typedef struct
{
    uint8_t write_buffer[4096];
    const uint32_t start;
    const uint32_t len;
    const uint32_t sample_size;
    volatile uint16_t sector_index;
    volatile uint16_t buffer_index;
} sample_store_t;


/**
 * @brief 
 * 
 * @return status_e 
 */
status_e sample_store_create(sample_store_t *instance);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e sample_store_reset(sample_store_t *instance);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e sample_store_append(sample_store_t *instance, const void *data, uint32_t len);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e sample_store_read(sample_store_t *instance, uint32_t index, void *data, uint32_t len, uint32_t count);

#endif
