/**
 * @file    command_handler.h
 * @author  Austin Wolf
 * @brief
 */

#include "imu_service_encode.h"
#include "ble_imu_encode.h"
#include "encoder.h"


/**
 * @see imu_service_encode.h
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

    ENCODER_DECODE_UINT8(&decoder, &command->type);

    switch (command->type)
    {
        case COMMAND_STREAM:
            ENCODER_DECODE_UINT8(&decoder, &command->stream.rate);
            ENCODER_DECODE_UINT8(&decoder, &command->stream.flags);
            ENCODER_DECODE_UINT16(&decoder, &command->stream.sampling_time);
            break;
        case COMMAND_RECORD:
            ENCODER_DECODE_UINT8(&decoder, &command->record.rate);
            ENCODER_DECODE_UINT8(&decoder, &command->record.flags);
            ENCODER_DECODE_UINT16(&decoder, &command->record.sampling_time);
            break;
        default:
            break;
    }

    return (decoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}

/**
 * @see imu_service_encode.h
 */
status_e imu_service_response_encode(const response_t *response, uint8_t *payload, uint8_t *len)
{
    encoder_t encoder = 
    {
        .buffer = payload,
        .len = MAX_PAYLOAD_LEN,
        .index = 0u,
        .overflow = false,
    };

    ENCODER_ENCODE_UINT8(&encoder, &response->type);
    ENCODER_ENCODE_UINT8(&encoder, &response->status);

    switch (response->type)
    {
        default:
            break;
    }

    *len = encoder.index;

    return (encoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}

/**
 * @see imu_service_encode.h
 */
status_e imu_service_sample_encode(imu_sample_t *sample, uint8_t *payload, uint8_t *len)
{
    encoder_t encoder = 
    {
        .buffer = payload,
        .len = MAX_PAYLOAD_LEN,
        .index = 0u,
        .overflow = false,
    };

    uint8_t type = UPDATE_SAMPLE;

    ENCODER_ENCODE_UINT8(&encoder, &type);
    ENCODER_ENCODE_UINT32(&encoder, &sample->timestamp);
    ENCODER_ENCODE_UINT8(&encoder, &sample->flags);
    encoder_encode_bytes(&encoder, (uint8_t*) sample->accel, sizeof(sample->accel));
    encoder_encode_bytes(&encoder, (uint8_t*) sample->gyro, sizeof(sample->gyro));
    encoder_encode_bytes(&encoder, (uint8_t*) sample->compass, sizeof(sample->compass));
    encoder_encode_bytes(&encoder, (uint8_t*) sample->quat, sizeof(sample->quat));

    *len = encoder.index;

    return (encoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}

/**
 * @see imu_service_encode.h
 */
status_e imu_service_state_update_encode(uint8_t current, uint8_t previous, uint8_t *payload, uint8_t *len)
{
    encoder_t encoder = 
    {
        .buffer = payload,
        .len = MAX_PAYLOAD_LEN,
        .index = 0u,
        .overflow = false,
    };

    uint8_t type = UPDATE_STATE_UPDATE;

    ENCODER_ENCODE_UINT8(&encoder, &type);
    ENCODER_ENCODE_UINT8(&encoder, &current);
    ENCODER_ENCODE_UINT8(&encoder, &previous);

    *len = encoder.index;

    return (encoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}