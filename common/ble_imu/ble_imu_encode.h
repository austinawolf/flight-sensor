  /*
 * @file    ble_imu_encode.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef BLE_IMU_MESSAGE_H_
#define BLE_IMU_MESSAGE_H_

#include <stdint.h>
#include "status.h"


/**
 * @brief 
 */
#define MAX_MESSAGE_LEN     (50u)

/**
 * @brief 
 */
#define MAX_PAYLOAD_LEN     (47u)

/**
 * @brief
 */
typedef enum
{
    BLE_IMU_MESSAGE_COMMAND,
    BLE_IMU_MESSAGE_RESPONSE,
    BLE_IMU_MESSAGE_UPDATE,
    BLE_IMU_MESSAGE_MAX_VALUE,
} ble_imu_message_e;

/**
 * @brief
 */
typedef struct 
{
    ble_imu_message_e type;
    uint8_t payload[MAX_PAYLOAD_LEN];
    uint8_t len;
} ble_imu_message_t;

/**
 * @brief
 */
status_e ble_imu_encode_message(const ble_imu_message_t *message, uint8_t *buffer, uint8_t *len);

/**
 * @brief
 */
status_e ble_imu_decode_message(const uint8_t *buffer, uint8_t len, ble_imu_message_t *message);

#endif
