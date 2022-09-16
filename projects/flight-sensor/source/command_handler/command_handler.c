/**
 * @file    command_handler.c
 * @author  Austin Wolf
 * @brief
 */
#include "command_handler.h"
#include "logger.h"
#include "state_machine.h"
#include "session_manager.h"

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

static void _load_success_response(command_code_e command, response_t *response, uint16_t *response_len)
{
    response->preamble = RESPONSE_PREAMBLE;
    response->command = command;
    if (*response_len == 0)
    {
        *response_len = HEADER_LENGTH;
    }

    return;
}

/**
 * @see command_handler.h
 */
status_e command_handler_process(command_t *command, uint16_t command_len, response_t *response, uint16_t *response_len)
{
    uint32_t status = STATUS_ERROR_INVALID_VALUE;

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

    switch (command->command)
    {
        case COMMAND_CODE_GET_STATUS:
            status = session_manager_get_status();
            break;
        case COMMAND_CODE_START_SAMPLING:
            status = session_manager_start_sampling(command->params.start.rate, 
                                                    command->params.start.flags,
                                                    command->params.start.destination,
                                                    command->params.start.sampling_time);
            break;        
        case COMMAND_CODE_STOP_SAMPLING:
            status = session_manager_stop_sampling();
            break;  
        case COMMAND_CODE_START_PLAYBACK:
            status = session_manager_start_playback();           
            break;
        case COMMAND_CODE_STOP_PLAYBACK:
            status = session_manager_stop_playback();         
            break;
        case COMMAND_CODE_CALIBRATE:
            status = session_manager_calibrate();         
            break;
        default:
            break;
    }

    if (status == STATUS_OK)
    {
        _load_success_response(command->command, response, response_len);
    }
    else
    {
        _load_error_response(response, response_len, status);
    }

    return STATUS_OK;
}
