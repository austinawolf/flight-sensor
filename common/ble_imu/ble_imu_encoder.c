/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include <string.h>
#include "ble_imu_encoder.h"


void ble_imu_encode_response(const response_payload_t *response, uint8_t response_len, ble_imu_packet_t *packet, uint8_t *len)
{
    const ble_imu_packet_header_t header = {.preamble = IMU_PREAMBLE_RESPONSE};
    memcpy(&packet->header, &header, sizeof(ble_imu_packet_header_t));
    memcpy(&packet->response, response, sizeof(response_payload_t));
    *len = sizeof(ble_imu_packet_header_t) + response_len;
}

void ble_imu_encode_state_update(session_state_e current, session_state_e previous, ble_imu_packet_t *packet, uint8_t *len)
{
    packet->header.preamble = (uint8_t) IMU_PREAMBLE_STATE_UPDATE;
    packet->state_update.current = (uint8_t) current;
    packet->state_update.previous = (uint8_t) previous;
    *len = sizeof(ble_imu_packet_header_t) + sizeof(packet->state_update);
}

void ble_imu_encode_data(const imu_sample_t *sample, uint16_t packet_index, ble_imu_packet_t *packet, uint8_t *len)
{
    packet->header.preamble = (uint8_t) IMU_PREAMBLE_DATA;
    packet->data.timestamp = sample->timestamp;
    packet->data.flags = sample->flags;
    packet->data.index = packet_index;

    memcpy(&packet->data.accel, &sample->accel, sizeof(sample->accel));
    memcpy(&packet->data.gyro, &sample->gyro, sizeof(sample->gyro));
    memcpy(&packet->data.compass, &sample->compass, sizeof(sample->compass));
    memcpy(&packet->data.quat, &sample->quat, sizeof(sample->quat));

    *len = sizeof(ble_imu_packet_header_t) + sizeof(packet->data);
}

void ble_imu_decode_command(const ble_imu_packet_t *packet, uint8_t packet_len, command_payload_t *command, uint8_t *len)
{
    memcpy(command, &packet->command, sizeof(command_payload_t));
    *len = packet_len - sizeof(ble_imu_packet_header_t);
}