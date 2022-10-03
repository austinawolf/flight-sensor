/**
 * @file    ble_imu_types.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef BLE_IMU_TYPES_H_
#define BLE_IMU_TYPES_H_

#include <stdint.h>
#include <stdbool.h>


/**
 * @brief Possible preambles to a packet
 */
typedef enum
{
    BLE_IMU_PREAMBLE_COMMAND = 0xAA,
    BLE_IMU_PREAMBLE_RESPONSE = 0xBB,
    BLE_IMU_PREAMBLE_STATE_UPDATE = 0xCC,
    BLE_IMU_PREAMBLE_DATA = 0xDD,
} ble_imu_preamble_t;

/**
 * @brief Possible command opcodes
 */
typedef enum
{
    COMMAND_CODE_GET_STATUS,
    COMMAND_CODE_STREAM,
    COMMAND_CODE_RECORD,
    COMMAND_CODE_PLAYBACK,
    COMMAND_CODE_STOP,
    COMMAND_CODE_CALIBRATE,
    COMMAND_CODE_ERROR,
    COMMAND_CODE_MAX_VALUE,
} ble_imu_opcode_e;

/**
 * @brief Definition of command packet
 */
typedef struct
{
    uint8_t opcode;
    union {
        struct {
            uint8_t rate;
            uint8_t flags;
            uint8_t sampling_time;
        } stream;
        struct {
            uint8_t rate;
            uint8_t flags;
            uint8_t stream_enable;
            uint8_t sampling_time;
        } record;
    };
} __attribute__((packed)) command_payload_t;

/**
 * @brief Definition of response packet
 */
typedef struct
{
    uint8_t opcode;
    uint8_t status;
} __attribute__((packed)) response_payload_t;

/**
 * @brief Definition of state update packet
 */
typedef struct
{
    uint8_t current;
    uint8_t previous; 
} __attribute__((packed)) state_update_payload_t;

/**
 * @brief Definition of data packet
 */
typedef struct
{
    uint16_t index;
    uint32_t timestamp;
    uint8_t flags;
    int32_t quat[4];
    int16_t gyro[3];
    int16_t accel[3];
    int16_t compass[3];
} __attribute__((packed)) data_payload_t;

/**
 * @brief Definition of header to all packets
 */
typedef struct
{
    uint8_t preamble;
} __attribute__((packed)) ble_imu_packet_header_t;

/**
 * @brief Definition of IMU packet
 */
typedef struct
{
    ble_imu_packet_header_t header;
    union
    {
        command_payload_t command;
        response_payload_t response;
        state_update_payload_t state_update;
        data_payload_t data;
    };
} __attribute__((packed)) ble_imu_packet_t;

#endif
