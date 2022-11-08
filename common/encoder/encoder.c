/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include <string.h>
#include "encoder.h"


/**
 * @see encode.h
 */
void encoder_encode_uint8(encode_buffer_t *buffer, uint8_t data)
{
    (void) memcpy(buffer->buffer, &data, sizeof(data));
    buffer->index += sizeof(data);
}

/**
 * @brief 
 * 
 */
void encoder_decode_uint8(encode_buffer_t *buffer, uint8_t *data)
{
    (void) memcpy(data, &buffer[buffer->index], sizeof(*data));
    buffer->index -= sizeof(*data);
}