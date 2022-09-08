/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef COMMAND_TYPES_H_
#define COMMAND_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


#define COMMAND_PREAMBLE        (0xAA)
#define RESPONSE_PREAMBLE       (0xBB)

#define HEADER_LENGTH           (sizeof(uint8_t) + sizeof(command_code_e))


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
} __attribute__((packed)) command_params_start_t;

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
} __attribute__((packed)) command_t;

/**
 * @brief 
 * 
 */
typedef struct
{
    uint32_t asdf;
} __attribute__((packed)) response_params_get_status_t;

/**
 * @brief 
 * 
 */
typedef struct
{
    status_e status;
} __attribute__((packed)) response_params_error_t;

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
} __attribute__((packed)) response_t;

#endif
