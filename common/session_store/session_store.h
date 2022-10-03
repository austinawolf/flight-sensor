/**
 * @file    session_store.h
 * @author  Austin Wolf
 * @brief   Double buffered data store of sample data saved in flash
 */

#ifndef SAMPLE_STORE_H_
#define SAMPLE_STORE_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu.h"


/**
 * @brief Initializes the module
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_store_create(void);

/**
 * @brief Prepares session store to accept data
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_store_open(void);

/**
 * @brief Flushes any buffered data and allows data to be read
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_store_close(void);

/**
 * @brief Appends a sample to the session store, must be open
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_store_append(const imu_sample_t *data);

/**
 * @brief Reads sample from session store, must be closed
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_store_read(uint32_t index, imu_sample_t *data);

/**
 * @brief Checks if session store is busy
 * 
 * @return true if busy
 * @return false if idle
 */
bool session_store_is_busy(void);

#endif
