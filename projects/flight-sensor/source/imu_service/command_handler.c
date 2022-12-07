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
 * @see command_handler.h
 */
status_e command_handler_process(imu_command_t *command, imu_response_t *response)
{
    response->type = command->type;

    switch (command->type)
    {
        case COMMAND_GET_STATUS:
            response->status = session_manager_get_status();
            break;
        case COMMAND_STREAM:
            response->status = session_manager_start_stream(command->stream.rate, 
                                                  command->stream.flags,
                                                  command->stream.sampling_time);
            break;        
        case COMMAND_RECORD:
            response->status = session_manager_start_recording(command->record.rate, 
                                                     command->record.flags,
                                                     false,
                                                     command->record.sampling_time);
            break;  
        case COMMAND_PLAYBACK:
            response->status = session_manager_start_playback();           
            break;
        case COMMAND_STOP:
            response->status = session_manager_stop();         
            break;
        case COMMAND_CALIBRATE:
            response->status = session_manager_calibrate();         
            break;
        default:
            break;
    }

    return STATUS_OK;
}
