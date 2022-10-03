
/**
 * @file    ble_helper.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef BLE_HELPER_H_
#define BLE_HELPER_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu.h"
#include "session_manager.h"


/**
 * @brief Types of events generate by the BLE helper module
 * 
 */
typedef enum
{
    BLE_HELPER_EVENT_IDLE,
    BLE_HELPER_EVENT_ADVERTISING,
    BLE_HELPER_EVENT_CONNECTED,
    BLE_HELPER_EVENT_DISCONNECTED,
    BLE_HELPER_EVENT_NOTIF_TX_COMPLETE,
} ble_helper_event_e;

/**
 * @brief Definition of BLE helper event handler callback
 */
typedef void (*ble_helper_event_handler_t) (ble_helper_event_e event);

/**
 * @brief Initializes the BLE helper module
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e ble_helper_create(void);

/**
 * @brief Register callback to receive BLE Events
 * 
 * @param event_handler 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e ble_helper_register_callback(ble_helper_event_handler_t event_handler);

/**
 * @brief Start advertising
 * 
 * @param erase_bonds true to erase bonds, false otherwise
 */
void ble_helper_advertising_start(bool erase_bonds);

/**
 * @brief Send IMU sample to central over BLE IMU service
 * 
 * @param sample sample to sent
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e ble_helper_sample_send(imu_sample_t *sample);

/**
 * @brief Send session manager state update to central
 * 
 * @param current new session manager state
 * @param previous last session manager state
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e ble_helper_send_state_update(session_state_e current, session_state_e previous);

#endif
