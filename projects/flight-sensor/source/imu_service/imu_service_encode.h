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


/**
 * @brief Processes a new command that has been received
 */
status_e imu_service_command_decode(uint8_t *payload, uint8_t len, command_t *command);

status_e imu_service_response_encode(const response_t *response, uint8_t *payload, uint8_t len);

#endif
