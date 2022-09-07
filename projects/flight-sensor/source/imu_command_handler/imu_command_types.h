/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef IMU_COMMAND_TYPES_H_
#define IMU_COMMAND_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define COMMAND_PREAMBLE    (0xAA)
#define RESPONSE_PREAMBLE    (0xBB)

#define HEADER_LENGTH       (sizeof(uint8_t) + sizeof(command_code_e))


/**
 * @brief 
 * 
 */
typedef enum
{
    COMMAND_CODE_GET_STATUS,
    COMMAND_CODE_START_SAMPLING,
    COMMAND_CODE_STOP_SAMPLING,
    COMMAND_CODE_START_PLAYBACK,
    COMMAND_CODE_STOP_PLAYBACK,
    COMMAND_CODE_ERROR,
    COMMAND_CODE_MAX_VALUE,
} command_code_e;

/**
 * @brief 
 * 
 */
typedef struct
{
    uint8_t asdf;
} command_params_start_t;

/**
 * @brief 
 * 
 */
typedef struct
{
    uint8_t preamble;
    command_code_e command;
    union
    {
        command_params_start_t start;
    } params;
} imu_command_t;

/**
 * @brief 
 * 
 */
typedef struct
{
    uint32_t asdf;
} response_params_get_status_t;

/**
 * @brief 
 * 
 */
typedef struct
{
    status_e status;
} response_params_error_t;

/**
 * @brief 
 * 
 */
typedef struct
{
    uint8_t preamble;
    command_code_e command;
    union
    {
        response_params_get_status_t get_status;
        response_params_error_t error;
    } params;
} imu_response_t;

#endif
