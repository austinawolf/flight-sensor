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
 * Helper function to load response packet
 */
static void _load_response(ble_imu_opcode_e opcode, status_e status, response_payload_t *response, uint8_t *response_len)
{
    response->opcode = opcode;
    *response_len = sizeof(response_payload_t);
    return;
}

/**
 * @see command_handler.h
 */
status_e command_handler_process(command_payload_t *command, uint8_t command_len, response_payload_t *response, uint8_t *response_len)
{
    uint32_t status = STATUS_ERROR_INVALID_VALUE;

    if (command_len > sizeof(command_payload_t))
    {
        _load_response(command->opcode, STATUS_ERROR_INVALID_LENGTH, response, response_len);
        return STATUS_OK;
    }

    if (command->opcode > COMMAND_CODE_MAX_VALUE)
    {
        _load_response(command->opcode, STATUS_ERROR_INVALID_VALUE, response, response_len);
        return STATUS_OK;
    }

    switch (command->opcode)
    {
        case COMMAND_CODE_GET_STATUS:
            status = session_manager_get_status();
            break;
        case COMMAND_CODE_STREAM:
            status = session_manager_start_stream(command->stream.rate, 
                                                  command->stream.flags,
                                                  command->stream.sampling_time);
            break;        
        case COMMAND_CODE_RECORD:
            status = session_manager_start_recording(command->record.rate, 
                                                    command->record.flags,
                                                    command->record.stream_enable,
                                                    command->record.sampling_time);
            break;  
        case COMMAND_CODE_PLAYBACK:
            status = session_manager_start_playback();           
            break;
        case COMMAND_CODE_STOP:
            status = session_manager_stop();         
            break;
        case COMMAND_CODE_CALIBRATE:
            status = session_manager_calibrate();         
            break;
        default:
            break;
    }

    _load_response(command->opcode, status, response, response_len);

    return STATUS_OK;
}
