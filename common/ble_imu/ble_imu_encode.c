  /*
 * @file    ble_imu_encode.h
 * @author  Austin Wolf
 * @brief
 */


#include <stdint.h>
#include "status.h"
#include "ble_imu_encode.h"
#include "logger.h"
#include "encoder.h"


/**
 * @brief
 */
status_e ble_imu_encode_message(const ble_imu_message_t *message, uint8_t *buffer, uint8_t *len)
{
    encoder_t encoder = 
    {
        .buffer = buffer,
        .len = MAX_MESSAGE_LEN,
        .index = 0,
        .overflow = false,
    };

    ENCODER_ENCODE_UINT8(&encoder, &message->type);
    ENCODER_ENCODE_UINT8(&encoder, &message->sequence);
    encoder_encode_bytes(&encoder, message->payload, message->len);

    *len = encoder.index;

    return (encoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}

/**
 * @brief
 */
status_e ble_imu_decode_message(const uint8_t *buffer, uint8_t len, ble_imu_message_t *message)
{
    decoder_t decoder =
    {
        .buffer = buffer,
        .len = len,
        .index = 0u,
    };

    ENCODER_DECODE_UINT8(&decoder, &message->type);
    ENCODER_DECODE_UINT8(&decoder, &message->sequence);
    encoder_decode_remaining(&decoder, message->payload, &message->len);

    return (decoder.overflow) ? STATUS_ERROR_INVALID_LENGTH : STATUS_OK;
}
