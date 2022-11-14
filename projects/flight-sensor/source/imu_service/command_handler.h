/**
 * @file    command_handler.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "command_handler.h"
#include "imu_service.h"

/**
 * @brief Processes a new command that has been received
 * 
 * @param command Command to process
 * @param response Response to the processed command
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e command_handler_process(command_t *command, response_t *response);

#endif
