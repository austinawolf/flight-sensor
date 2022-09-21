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


void ble_imu_encode_response(const response_payload_t *response, uint8_t response_len, ble_imu_packet_t *packet, uint8_t *len);

void ble_imu_encode_state_update(session_state_e current, session_state_e previous, ble_imu_packet_t *packet, uint8_t *len);

void ble_imu_encode_data(const imu_sample_t *sample, uint16_t packet_index, ble_imu_packet_t *packet, uint8_t *len);

void ble_imu_decode_command(const ble_imu_packet_t *packet, uint8_t packet_len, command_payload_t *command, uint8_t *len);


#endif
