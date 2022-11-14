/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include <string.h>
#include "encoder.h"


#define CHECK_OVERFLOW(__decoder__) do {                            \
                                        if (__decoder__->overflow)  \
                                        {                           \
                                            return;                 \
                                        }                           \
                                    } while(0)                      \

/**
 * @see encode.h
 */
void encoder_encode_uint8(encoder_t *encoder, uint8_t data)
{
    (void) memcpy(&encoder->buffer[encoder->index], &data, sizeof(data));
    encoder->index += sizeof(data);
}

/**
 * @brief 
 */
void encoder_decode_uint8(decoder_t *decoder, uint8_t *data)
{
    uint8_t len = sizeof(uint8_t);
    decoder->overflow = decoder->overflow || ((len + decoder->index) > decoder->len);

    if (!decoder->overflow)
    {
        (void) memcpy(data, &decoder->buffer[decoder->index], len);
        decoder->index += len;
    }
}

/**
 * @brief 
 */
void encoder_decode_uint16(decoder_t *decoder, uint16_t *data)
{
    uint8_t len = sizeof(uint16_t);
    decoder->overflow = decoder->overflow || ((len + decoder->index) > decoder->len);

    if (!decoder->overflow)
    {
        (void) memcpy(data, &decoder->buffer[decoder->index], len);
        decoder->index += len;
    }
}

/**
 * @brief 
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

void encoder_decode_remaining(decoder_t *decoder, uint8_t *data, uint8_t *len)
{
    if (decoder->len < decoder->index)
    {
        *len = 0u;
        return;
    }

    *len = decoder->len - decoder->index;

    (void) memcpy(data, &decoder->buffer[decoder->index], *len);
}