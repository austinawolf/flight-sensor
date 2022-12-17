/**
 * @file    timestamp.h
 * @author  Austin Wolf
 * @brief   keeps milliseconds since boot
 */

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


/**
 * @brief Initializes and starts the timestamp timer
 * 
 * @return status_e 
 */
status_e timestamp_create(void);

/**
 * @brief Stops the timestamp timer
 * 
 * @return status_e 
 */
status_e timestamp_destory(void);

/**
 * @brief Gets the number of milliseconds since boot
 * 
 * @return uint32_t 
 */
uint32_t timestamp_get(void);

#endif
