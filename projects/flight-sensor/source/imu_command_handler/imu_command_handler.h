/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef IMU_COMMAND_H_
#define IMU_COMMAND_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu_command_types.h"


#define MAX_COMMAND_LENGTH (sizeof(imu_command_t))

/**
 * @brief 
 * 
 * @param command 
 * @param command_len 
 * @param response 
 * @param reponse_len 
 * @return status_e 
 */
status_e imu_command_handler_process(imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *response_len);

#endif
