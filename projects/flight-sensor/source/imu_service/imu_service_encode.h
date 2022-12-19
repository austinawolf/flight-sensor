/**
 * @file    imu_service_encode.h
 * @author  Austin Wolf
 * @brief   Encodes/decodes IMU service commands, reponses and notifications
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu_service.h"
#include "imu.h"


/**
 * @brief Decodes command to imu_command_t structure
 * 
 * @param payload pointer to payload to decode
 * @param len length of payload to decode
 * @param command pointer to IMU command
 * @return STATUS_OK on success
 */
status_e imu_service_command_decode(uint8_t *payload, uint8_t len, imu_command_t *command);

/**
 * @brief Encodes imu_response_t structure into payload
 * 
 * @param response pointer to IMU response data
 * @param payload pointer to store payload
 * @param len pointer to store length of payload
 * @return STATUS_OK on success
 */
status_e imu_service_response_encode(const imu_response_t *response, uint8_t *payload, uint8_t *len);

/**
 * @brief Encodes imu_notification_t structure into payload
 * 
 * @param response pointer to IMU notification data
 * @param payload pointer to store payload
 * @param len pointer to store length of payload
 * @return STATUS_OK on success
 */
status_e imu_service_notification_encode(const imu_notification_t *notification, uint8_t *payload, uint8_t *len);

#endif
