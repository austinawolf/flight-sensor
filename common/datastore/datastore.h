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


status_e datastore_create(void);

status_e datastore_reset(void);

status_e datastore_get_cal_data(icm20948_calibration_t *data);

status_e datastore_set_cal_data(const icm20948_calibration_t *data);

status_e datastore_flush(void);

#endif
