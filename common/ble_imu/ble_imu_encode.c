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
    return STATUS_OK;
}

/**
 * @brief
 */
status_e ble_imu_decode_message(const uint8_t *buffer, uint8_t len, ble_imu_message_t *message)
{
    LOG_HEX_DUMP(buffer, len);

    decoder_t decoder =
    {
        .buffer = buffer,
        .len = len,
        .index = 0u,
    };

    ENCODER_DECODE_UINT8(&decoder, &message->type);
    encoder_decode_remaining(&decoder, message->payload, &message->len);

    return STATUS_OK;
}
