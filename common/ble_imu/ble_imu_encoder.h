/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef BLE_IMU_ENCODER_H_
#define BLE_IMU_ENCODER_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "ble_imu_types.h"
#include "imu.h"
#include "session_manager.h"


/**
 * @brief Encode a response into a packet
 * 
 * @param response pointer to response to encode 
 * @param response_len length of response
 * @param packet pointer to packet to store data
 * @param len length of packet
 */
void ble_imu_encode_response(const response_payload_t *response, uint8_t response_len, ble_imu_packet_t *packet, uint8_t *len);

/**
 * @brief Encode a state update into a packet
 * 
 * @param current new session manager state
 * @param previous last session manager state
 * @param packet pointer to packet to store data
 * @param len length of packet
 */
void ble_imu_encode_state_update(session_state_e current, session_state_e previous, ble_imu_packet_t *packet, uint8_t *len);

/**
 * @brief Encodes an IMU sample into a packet
 * 
 * @param sample sample to encode
 * @param packet_index index of data packet
 * @param packet pointer to packet to store data
 * @param len length of packet
 */
void ble_imu_encode_data(const imu_sample_t *sample, uint16_t packet_index, ble_imu_packet_t *packet, uint8_t *len);

/**
 * @brief Decodees a packet into a command
 * 
 * @param packet pointer to packet to decode
 * @param packet_len length of packet
 * @param command pointer to command to store decode info
 * @param len length of decoded command
 */
void ble_imu_decode_command(const ble_imu_packet_t *packet, uint8_t packet_len, command_payload_t *command, uint8_t *len);


#endif
