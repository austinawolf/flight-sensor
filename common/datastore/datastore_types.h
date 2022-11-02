/**
 * @file    
 * @author  Austin Wolf
 * @brief
 */

#ifndef DATASTORE_TYPES_H
#define DATASTORE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "icm20948.h"

/**
 *
 */
#define DATASTORE_PREAMBLE  (0xDEADBEEF)


/**
 *
 */
typedef struct
{
    uint32_t preamble;
    icm20948_calibration_t icm20948_calibration;
} datastore_t;

#endif
