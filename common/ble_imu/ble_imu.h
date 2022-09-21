#ifndef BLE_IMU_H__
#define BLE_IMU_H__

#include <stdint.h>
#include <stdbool.h>
#include "queue.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "imu.h"
#include "status.h"
#include "session_manager.h"
#include "ble_imu_uuids.h"
#include "ble_imu_types.h"


#define RETRY_QUEUE_LEN (5)
#define BLE_IMU_BLE_OBSERVER_PRIO           3

/**
 * Macro for defining a ble_imu instance
 */
#define BLE_IMU_DEF(_name)                                                                          \
static ble_imu_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_IMU_BLE_OBSERVER_PRIO,                                                     \
                     ble_imu_on_ble_evt, &_name)

// Forward declaration of the ble_imu_t type.
typedef struct ble_imu_s ble_imu_t;

typedef struct
{
    uint8_t unused;                       
} ble_imu_init_t;

struct ble_imu_s
{
    uint16_t                     service_handle;
    ble_gatts_char_handles_t     command_handles;
    ble_gatts_char_handles_t     data_handles;
    uint16_t                     conn_handle;
    uint8_t                      uuid_type;
    uint16_t                     packet_index;
    queue_t                      retry_queue;
    packet_retry_t               retry_buffer[RETRY_QUEUE_LEN];
};

status_e ble_imu_init(ble_imu_t * p_imu, ble_imu_init_t const * p_imu_init);

status_e ble_imu_sample_send(ble_imu_t * p_imu, imu_sample_t *sample);

status_e ble_imu_send_state_update(ble_imu_t * p_imu, session_state_e current, session_state_e previous);

void ble_imu_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

#endif
