/**
 * @file    command_handler.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu_service.h"
#include "imu.h"


/**
 * @brief
 */
status_e imu_service_command_decode(uint8_t *payload, uint8_t len, imu_command_t *command);

/**
 * @brief
 */
status_e imu_service_response_encode(const imu_response_t *response, uint8_t *payload, uint8_t *len);

/**
 * @brief
 */
status_e imu_service_notification_encode(const imu_notification_t *notification, uint8_t *payload, uint8_t *len);

#endif
