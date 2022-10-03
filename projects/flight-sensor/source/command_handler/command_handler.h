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
 * @brief Processes a new command that has been received
 * 
 * @param command Command to process
 * @param command_len Length of command, in bytes
 * @param response Response to the processed command
 * @param response_len Length of reponse, in bytes
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e command_handler_process(command_payload_t *command, uint8_t command_len, response_payload_t *response, uint8_t *response_len);

#endif
