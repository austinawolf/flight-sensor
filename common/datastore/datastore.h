/**
 * @file    
 * @author  Austin Wolf
 * @brief
 */

#ifndef DATASTORE_H
#define DATASTORE_H

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "datastore_types.h"


/**
 * @brief Creates datastore
 * 
 * @return STATUS_OK on success
 */
status_e datastore_create(void);

/**
 * @brief Sets datastore to defaults
 * 
 * @return STATUS_OK on success
 */
status_e datastore_reset(void);

/**
 * @brief Get calibration data
 * 
 * @param data pointer to store calibration data
 * @return STATUS_OK on success
 */
status_e datastore_get_cal_data(icm20948_calibration_t *data);

/**
 * @brief Sets calibration data
 * 
 * @param data pointer to calibration data to store
 * @return STATUS_OK on success
 * 
 * @note call #datastore_flush to commit to flash
 */
status_e datastore_set_cal_data(const icm20948_calibration_t *data);

/**
 * @brief Writes shadow copy to flash
 * 
 * @return STATUS_OK on success
 */
status_e datastore_flush(void);

#endif
