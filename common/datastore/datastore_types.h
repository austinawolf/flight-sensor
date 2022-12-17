/**
 * @file    datastore_types.h
 * @author  Austin Wolf
 * @brief   Types used by the datastore module
 */

#ifndef DATASTORE_TYPES_H
#define DATASTORE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "icm20948.h"


/**
 * @brief Definition of datastore contents to save/load
 */
typedef struct
{
    uint32_t preamble;
    uint32_t version;
    icm20948_calibration_t icm20948_calibration;
} datastore_t;

#endif
