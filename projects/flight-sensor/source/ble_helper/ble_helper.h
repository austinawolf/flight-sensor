
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

typedef enum
{
    BLE_HELPER_EVENT_IDLE,
    BLE_HELPER_EVENT_ADVERTISING,
    BLE_HELPER_EVENT_CONNECTED,
    BLE_HELPER_EVENT_DISCONNECTED,
} ble_helper_event_e;


typedef void (*ble_helper_event_handler_t) (ble_helper_event_e event);


/**
 * @brief 
 * 
 * @return status_e 
 */
status_e ble_helper_create(ble_helper_event_handler_t event_handler);

void ble_helper_advertising_start(bool erase_bonds);

status_e ble_helper_sample_send(imu_sample_t *sample);

#endif
