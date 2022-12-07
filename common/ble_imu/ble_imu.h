/**
 * @file    ble_imu.h
 * @author  Austin Wolf
 * @brief
 */

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
#include "ble_imu_encode.h"


/**
 * @brief Number of notifications that can be buffered to retry later
 * 
 * @note Not used for IMU samples
 */
#define RETRY_QUEUE_LEN (5)

/**
 * @brief Service observer priority
 * 
 * @note Don't add parenthesis to this value or the macro won't work
 */
#define BLE_IMU_BLE_OBSERVER_PRIO           3

/**
 * Macro for defining a ble_imu instance
 */
#define BLE_IMU_DEF(_name)                          \
static ble_imu_t _name;                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                 \
                     BLE_IMU_BLE_OBSERVER_PRIO,     \
                     ble_imu_on_ble_evt, &_name)

/* Forward declaration of the ble_imu_t type. */
typedef struct ble_imu_s ble_imu_t;

/**
 * @brief Structure to store packet in retry buffer 
 */
typedef struct
{
    uint8_t buffer[MAX_MESSAGE_LEN];
    uint8_t len;
} ble_imu_retry_t;


/**
 * @brief
 */
typedef void (*ble_imu_command_callback_t)(uint8_t *command_payload, uint8_t command_len, uint8_t *response_payload, uint8_t *response_len);

/**
 * @brief Definition of BLE IMU control structure
 */
struct ble_imu_s
{
    uint16_t                     service_handle;
    ble_gatts_char_handles_t     command_handles;
    ble_gatts_char_handles_t     data_handles;
    uint16_t                     conn_handle;
    uint8_t                      uuid_type;
    uint16_t                     packet_index;
    queue_t                      retry_queue;
    ble_imu_retry_t              retry_buffer[RETRY_QUEUE_LEN];
    ble_imu_command_callback_t   on_command;
    void                         *on_command_context;
};

/**
 * @brief Initialies a BLE IMU control structure
 * 
 * @param p_imu 
 * @param p_imu_init 
 * @return status_e 
 */
status_e ble_imu_create(void);

/**
 * @brief Sends a single IMU sample
 * 
 * @param p_imu pointer to a IMU control structure
 * @param sample pointer to IMU sample to send
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e ble_imu_send_update(uint8_t *payload, uint8_t len, bool retry);

/**
 * @brief
 * 
 * @param callback 
 * @param context 
 * @return status_e 
 */
void ble_imu_on_command(ble_imu_command_callback_t callback, void *context);

/**
 * @brief Called when a new BLE event is received
 * 
 * @param p_ble_evt pointer to BLE event structure
 * @param p_context context pointer to BLE IMU control structure
 */
void ble_imu_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


#endif
