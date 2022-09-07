/**
 * @file    command_handler.c
 * @author  Austin Wolf
 * @brief
 */
#include "command_handler.h"
#include "logger.h"
#include "state_machine.h"

/**
 * @brief 
 * 
 */
#define DEFINE_COMMAND_PROCESSOR(_code, _callback)  [_code] (command_processor_t) {.code=_code, .callback=_callback}

/**
 * @brief 
 * 
 */
typedef void (*command_callback_t) (command_t *command, uint16_t command_len, response_t *response, uint16_t *response_len);


typedef struct 
{
    command_code_e code;
    command_callback_t callback;
} command_processor_t;


static void _load_error_response(response_t *response, uint16_t *response_len, status_e status)
{
    response->preamble = RESPONSE_PREAMBLE;
    response->command = COMMAND_CODE_ERROR;
    response->params.error.status = status;
    *response_len = (sizeof(HEADER_LENGTH) + sizeof(response->params.error));

    return;
}

static void _get_status(command_t *command, uint16_t command_len, response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    LOG_INFO("Get status");
}

static void _start_sampling(command_t *command, uint16_t command_len, response_t *response, uint16_t *reponse_len)
{
    if (command_len != (HEADER_LENGTH + sizeof(command->params.start)))
    {
        return;
    }

    event_t event = 
    {
        .event = EVENT_START_SAMPLING,
        .start_sampling = 
        {
            .rate = 0,
            .flags = 0,
            .destination = 0,
            .time = 0,
        }
    };
    state_machine_process(&event);
}

static void _stop_sampling(command_t *command, uint16_t command_len, response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    event_t event = 
    {
        .event = EVENT_STOP_SAMPLING,
    };
    state_machine_process(&event);
}

static void _start_playback(command_t *command, uint16_t command_len, response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    event_t event = 
    {
        .event = EVENT_START_PLAYBACK,
    };
    state_machine_process(&event);
}

static void _stop_playback(command_t *command, uint16_t command_len, response_t *response, uint16_t *reponse_len)
{
    if (command_len != HEADER_LENGTH)
    {
        return;
    }

    event_t event = 
    {
        .event = EVENT_STOP_PLAYBACK,
    };
    state_machine_process(&event);
}


static command_processor_t _processors[COMMAND_CODE_MAX_VALUE] = 
{
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_GET_STATUS, _get_status),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_START_SAMPLING, _start_sampling),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_STOP_SAMPLING, _stop_sampling),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_START_PLAYBACK, _start_playback),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_STOP_PLAYBACK, _stop_playback),
    DEFINE_COMMAND_PROCESSOR(COMMAND_CODE_ERROR, NULL),
};

/**
 * @see command_handler.h
 */
status_e command_handler_process(command_t *command, uint16_t command_len, response_t *response, uint16_t *response_len)
{
    if (command_len > sizeof(command_t))
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
