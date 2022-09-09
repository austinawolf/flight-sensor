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

typedef struct
{
    status_e (*get_status) (void);
    status_e (*start_sampling) (imu_sample_rate_e rate, uint8_t flags, uint8_t destination, uint8_t sampling_time);
    status_e (*stop_sampling) (void);
    status_e (*start_playback) (void);
    status_e (*stop_playback) (void);
    status_e (*calibrate) (void);
} command_handler_callbacks_t;


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

void command_handler_register_callbacks(const command_handler_callbacks_t *callbacks);

#endif
