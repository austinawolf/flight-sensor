/**
 * @file    command_handler.h
 * @author  Austin Wolf
 * @brief
 */

#include "imu_service_encode.h"
#include "encoder.h"


/**
 * @brief Processes a new command that has been received
 */
status_e imu_service_command_decode(uint8_t *payload, uint8_t len, command_t *command)
{
    decoder_t decoder = 
    {
        .buffer = payload,
        .len = len,
        .index = 0u,
        .overflow = false,
    };

    encoder_decode_uint8(&decoder, &command->type);

    switch (command->type)
    {
        case COMMAND_STREAM:
            encoder_decode_uint8(&decoder, &command->stream.rate);
            encoder_decode_uint8(&decoder, &command->stream.flags);
            encoder_decode_uint16(&decoder, &command->stream.sampling_time);
            break;
        case COMMAND_RECORD:
            encoder_decode_uint8(&decoder, &command->record.rate);
            encoder_decode_uint8(&decoder, &command->record.flags);
            encoder_decode_uint16(&decoder, &command->record.sampling_time);
            break;
        default:
            break;
    }

    return (decoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}

status_e imu_service_response_encode(const response_t *response, uint8_t *payload, uint8_t *len)
{
    return STATUS_OK;
}
