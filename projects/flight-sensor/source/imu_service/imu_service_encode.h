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


#define MAX_PAYLOAD_LEN     (50u)

/**
 * @brief
 */
status_e imu_service_command_decode(uint8_t *payload, uint8_t len, command_t *command);

/**
 * @brief
 */
status_e imu_service_response_encode(const response_t *response, uint8_t *payload, uint8_t *len);

/**
 * @brief
 */
status_e imu_service_sample_encode(imu_sample_t *sample, uint8_t *payload, uint8_t *len);

/**
 * @brief
 */
status_e imu_service_state_update_encode(uint8_t current, uint8_t previous, uint8_t *payload, uint8_t *len);

#endif
