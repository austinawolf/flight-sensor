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
#include "ble_imu_types.h"


/**
 *
 */
status_e command_handler_process(command_payload_t *command, uint8_t command_len, response_payload_t *response, uint8_t *response_len);

#endif
