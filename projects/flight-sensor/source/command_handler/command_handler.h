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
#include "command_types.h"


#define MAX_COMMAND_LENGTH (sizeof(command_t))

/**
 * @brief 
 * 
 * @param command 
 * @param command_len 
 * @param response 
 * @param reponse_len 
 * @return status_e 
 */
status_e command_handler_process(command_t *command, uint16_t command_len, response_t *response, uint16_t *response_len);

#endif
