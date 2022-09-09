/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef BLE_HELPER_H_
#define BLE_HELPER_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu.h"


/**
 * @brief 
 * 
 * @return status_e 
 */
status_e ble_helper_create(void);

void ble_helper_advertising_start(bool erase_bonds);

void ble_helper_sample_send(imu_sample_t *sample);

#endif
