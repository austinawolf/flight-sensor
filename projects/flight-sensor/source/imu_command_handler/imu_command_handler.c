/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include "imu_command_handler.h"
#include "logger.h"

/**
 * @brief 
 * 
 */
#define DEFINE_COMMAND_PROCESSOR(_code, _callback)  [_code] (command_processor_t) {.code=_code, .callback=_callback}

/**
 * @brief 
 * 
 */
typedef void (*imu_command_callback_t) (imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *response_len);


typedef struct 
{
    command_code_e code;
    imu_command_callback_t callback;
} command_processor_t;


static void _load_error_response(imu_response_t *response, uint16_t *response_len, status_e status)
{
    response->preamble = RESPONSE_PREAMBLE;
    response->command = COMMAND_CODE_ERROR;
    response->params.error.status = status;
    *response_len = (sizeof(HEADER_LENGTH) + sizeof(response->params.error));

    return;
}

static void _get_status(imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    LOG_INFO("Get status");
}

static void _start_sampling(imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *reponse_len)
{
    if (command_len != (HEADER_LENGTH + sizeof(command->params.start)))
    {
        return;
    }

    LOG_INFO("Start Sampling");
}

static void _stop_sampling(imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    LOG_INFO("Stop Sampling");
}

static void _start_playback(imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    LOG_INFO("Start Playback");
}

static void _stop_playback(imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    LOG_INFO("Stop Playback");
}


static command_processor_t _processors[COMMAND_CODE_MAX_VALUE] = 
{
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_GET_STATUS, _get_status),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_START_SAMPLING, _start_sampling),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_STOP_SAMPLING, _stop_sampling),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_START_SAMPLING, _start_playback),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_STOP_SAMPLING, _stop_playback),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_ERROR, NULL),
};

/**
 * @see imu_command_handler.h
 */
status_e imu_command_handler_process(imu_command_t *command, uint16_t command_len, imu_response_t *response, uint16_t *response_len)
{
    if (command_len > sizeof(imu_command_t))
    {
        _load_error_response(response, response_len, STATUS_ERROR_INVALID_LENGTH);
        return STATUS_OK;
    }

    if (command->preamble != COMMAND_PREAMBLE)
    {
        _load_error_response(response, response_len, STATUS_ERROR_INVALID_VALUE);
        return STATUS_OK;
    }

    if (command->command > COMMAND_CODE_MAX_VALUE)
    {
        _load_error_response(response, response_len, STATUS_ERROR_INVALID_VALUE);
        return STATUS_OK;
    }

    command_processor_t processor = _processors[command->command];
    if (processor.callback == NULL)
    {
        _load_error_response(response, response_len, STATUS_ERROR_INVALID_VALUE);
        return STATUS_OK;
    }

    processor.callback(command, command_len, response, response_len);

    return STATUS_OK;
}
