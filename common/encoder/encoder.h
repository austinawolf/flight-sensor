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
    bool overflow;
} encoder_t;

typedef struct
{
    const uint8_t *buffer;
    uint8_t len;
    uint8_t index;
    bool overflow;
} decoder_t;


void encoder_encode_uint8(encoder_t *encoder, uint8_t data);

void encoder_encode_uint16(encoder_t *encoder, uint16_t data);

void encoder_encode_uint32(encoder_t *encoder, uint32_t data);

void encoder_encode_int8(encoder_t *encoder, int8_t data);

void encoder_encode_int16(encoder_t *encoder, int16_t data);

void encoder_encode_int32(encoder_t *encoder, int32_t data);

void encoder_encode_bytes(encoder_t *encoder, uint8_t *data, uint8_t len);

void encoder_decode_uint8(decoder_t *decoder, uint8_t *data);

void encoder_decode_uint16(decoder_t *decoder, uint16_t *data);

void encoder_decode_uint32(decoder_t *decoder, uint32_t *data);

void encoder_decode_int8(decoder_t *decoder, int8_t *data);

void encoder_decode_int16(decoder_t *decoder, int16_t *data);

void encoder_decode_int32(decoder_t *decoder, int32_t *data);

void encoder_decode_bytes(decoder_t *decoder, uint8_t *data, uint8_t len);

void encoder_decode_remaining(decoder_t *decoder, uint8_t *data, uint8_t *len);

#endif
