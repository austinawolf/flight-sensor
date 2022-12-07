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

/**
 * @brief Type encoders
 */
#define ENCODER_ENCODE_UINT8(__encoder__, __data__)     (encoder_encode_bytes(__encoder__, __data__, sizeof(uint8_t)))
#define ENCODER_ENCODE_UINT16(__encoder__, __data__)    (encoder_encode_bytes(__encoder__, (uint8_t*) __data__, sizeof(uint16_t)))
#define ENCODER_ENCODE_UINT32(__encoder__, __data__)    (encoder_encode_bytes(__encoder__, (uint8_t*) __data__, sizeof(uint32_t)))
#define ENCODER_ENCODE_INT8(__encoder__, __data__)      (encoder_encode_bytes(__encoder__, (uint8_t*) __data__, sizeof(int8_t)))
#define ENCODER_ENCODE_INT16(__encoder__, __data__)     (encoder_encode_bytes(__encoder__, (uint8_t*) __data__, sizeof(int16_t)))
#define ENCODER_ENCODE_INT32(__encoder__, __data__)     (encoder_encode_bytes(__encoder__, (uint8_t*) __data__, sizeof(int32_t)))

/**
 * @brief Type decoders
 */
#define ENCODER_DECODE_UINT8(__decoder__, __data__)     (encoder_decode_bytes(__decoder__, __data__, sizeof(uint8_t)))
#define ENCODER_DECODE_UINT16(__decoder__, __data__)    (encoder_decode_bytes(__decoder__, (uint8_t*) __data__, sizeof(uint16_t)))
#define ENCODER_DECODE_UINT32(__decoder__, __data__)    (encoder_decode_bytes(__decoder__, (uint8_t*) __data__, sizeof(uint32_t)))
#define ENCODER_DECODE_INT8(__decoder__, __data__)      (encoder_decode_bytes(__decoder__, (uint8_t*) __data__, sizeof(int8_t)))
#define ENCODER_DECODE_INT16(__decoder__, __data__)     (encoder_decode_bytes(__decoder__, (uint8_t*) __data__, sizeof(int16_t)))
#define ENCODER_DECODE_INT32(__decoder__, __data__)     (encoder_decode_bytes(__decoder__, (uint8_t*) __data__, sizeof(int32_t)))



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

/**
 * @brief 
 * 
 * @param encoder 
 * @param data 
 * @param len 
 */
void encoder_encode_bytes(encoder_t *encoder, const uint8_t *data, uint8_t len);

/**
 * @brief 
 * 
 * @param decoder 
 * @param data 
 * @param len 
 */
void encoder_decode_bytes(decoder_t *decoder, uint8_t *data, uint8_t len);

/**
 * @brief
 * 
 * @param decoder 
 * @param data 
 * @param len 
 */
void encoder_decode_remaining(decoder_t *decoder, uint8_t *data, uint8_t *len);

#endif
