/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef ENCODE_H_
#define ENCODE_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


#define MAX_DATA_LEN    (50u)


typedef struct
{
    uint8_t *buffer;
    uint8_t len;
    uint8_t index;
} encode_buffer_t;


void encoder_encode_uint8(encode_buffer_t *buffer, uint8_t data);

void encoder_encode_uint16(encode_buffer_t *buffer, uint16_t data);

void encoder_encode_uint32(encode_buffer_t *buffer, uint32_t data);

void encoder_encode_int8(encode_buffer_t *buffer, int8_t data);

void encoder_encode_int16(encode_buffer_t *buffer, int16_t data);

void encoder_encode_int32(encode_buffer_t *buffer, int32_t data);

void encoder_encode_bytes(encode_buffer_t *buffer, uint8_t *data, uint8_t len);

void encoder_decode_uint8(encode_buffer_t *buffer, uint8_t *data);

void encoder_decode_uint16(encode_buffer_t *buffer, uint16_t *data);

void encoder_decode_uint32(encode_buffer_t *buffer, uint32_t *data);

void encoder_decode_int8(encode_buffer_t *buffer, int8_t *data);

void encoder_decode_int16(encode_buffer_t *buffer, int16_t *data);

void encoder_decode_int32(encode_buffer_t *buffer, int32_t *data);

void encoder_decode_bytes(encode_buffer_t *buffer, uint8_t *data, uint8_t len);

#endif
