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
status_e imu_service_command_decode(uint8_t *payload, uint8_t len, imu_command_t *command)
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
status_e imu_service_response_encode(const imu_response_t *response, uint8_t *payload, uint8_t *len)
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
status_e imu_service_notification_encode(const imu_notification_t *notification, uint8_t *payload, uint8_t *len)
{
    encoder_t encoder = 
    {
        .buffer = payload,
        .len = MAX_PAYLOAD_LEN,
        .index = 0u,
        .overflow = false,
    };

    uint8_t type = notification->type;
    ENCODER_ENCODE_UINT8(&encoder, &type);

    switch (notification->type)
    {
        case NOTIFICATION_STATE_UPDATE:
            ENCODER_ENCODE_UINT8(&encoder, &notification->state_update.current);
            ENCODER_ENCODE_UINT8(&encoder, &notification->state_update.previous);
            break;
        case NOTIFICATION_SAMPLE:
            ENCODER_ENCODE_UINT32(&encoder, notification->sample.sample.timestamp);
            ENCODER_ENCODE_UINT8(&encoder, &notification->sample.sample.flags);
            encoder_encode_bytes(&encoder, (uint8_t*) notification->sample.sample.accel, sizeof(notification->sample.sample.accel));
            encoder_encode_bytes(&encoder, (uint8_t*) notification->sample.sample.gyro, sizeof(notification->sample.sample.gyro));
            encoder_encode_bytes(&encoder, (uint8_t*) notification->sample.sample.compass, sizeof(notification->sample.sample.compass));
            encoder_encode_bytes(&encoder, (uint8_t*) notification->sample.sample.quat, sizeof(notification->sample.sample.quat));
            break;
        default:
            return STATUS_ERROR_INVALID_PARAM;
            break;
    }

    *len = encoder.index;

    return (encoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}
