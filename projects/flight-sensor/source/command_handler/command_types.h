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
#include "imu.h"


#define COMMAND_PREAMBLE        (0xAA)
#define RESPONSE_PREAMBLE       (0xBB)
#define STATE_UPDATE_PREAMBLE   (0xCC)


#define HEADER_LENGTH           (sizeof(uint8_t) + sizeof(command_code_e))


/**
 * @brief 
 * 
 */
typedef enum
{
    COMMAND_CODE_GET_STATUS,
    COMMAND_CODE_STREAM,
    COMMAND_CODE_RECORD,
    COMMAND_CODE_PLAYBACK,
    COMMAND_CODE_STOP,
    COMMAND_CODE_CALIBRATE,
    COMMAND_CODE_ERROR,
    COMMAND_CODE_MAX_VALUE,
} command_code_e;

/**
 * @brief 
 * 
 */
typedef struct
{
    imu_sample_rate_e rate;
    uint8_t flags;
    uint8_t sampling_time;
} __attribute__((packed)) command_params_stream_t;

/**
 * @brief 
 * 
 */
typedef struct
{
    imu_sample_rate_e rate;
    uint8_t flags;
    bool stream_enable;
    uint8_t sampling_time;
} __attribute__((packed)) command_params_record_t;

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
        command_params_stream_t stream;
        command_params_record_t record;
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

/**
 * @brief 
 * 
 */
typedef struct
{
    uint8_t preamble;
    uint8_t current;
    uint8_t previous;
} __attribute__((packed)) state_update_t;

#endif
