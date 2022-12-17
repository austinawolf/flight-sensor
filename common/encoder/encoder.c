/**
 * @file    encoder.c
 * @author  Austin Wolf
 * @brief   helper module to encode/decode data
 */

#include <string.h>
#include "encoder.h"


/**
 * @see encode.h
 */
void encoder_encode_bytes(encoder_t *encoder, const uint8_t *data, uint8_t len)
{
    encoder->overflow = encoder->overflow || ((len + encoder->index) > encoder->len);

    if (!encoder->overflow)
    {
        (void) memcpy(&encoder->buffer[encoder->index], data, len);
        encoder->index += len;
    }
}

/**
 * @see encode.h
 */
void encoder_decode_bytes(decoder_t *decoder, uint8_t *data, uint8_t len)
{
    decoder->overflow = decoder->overflow || ((len + decoder->index) > decoder->len);

    if (!decoder->overflow)
    {
        (void) memcpy(data, &decoder->buffer[decoder->index], len);
        decoder->index += len;
    }
}

/**
 * @see encode.h
 */
void encoder_decode_remaining(decoder_t *decoder, uint8_t *data, uint8_t *len)
{
    *len = (decoder->len > decoder->index) ? (decoder->len - decoder->index) : 0u;

    encoder_decode_bytes(decoder, data, *len);
}