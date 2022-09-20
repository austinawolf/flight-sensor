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
#include "imu.h"


/**
 * @brief 
 * 
 * @return status_e 
 */
status_e session_store_create(void);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e session_store_open(void);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e session_store_close(void);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e session_store_append(const imu_sample_t *data);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e session_store_read(uint32_t index, imu_sample_t *data);

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool session_store_is_busy(void);

#endif
