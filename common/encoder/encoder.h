/**
 * @file    encoder.h
 * @author  Austin Wolf
 * @brief   helper module to encode/decode data
 */

#ifndef ENCODE_H_
#define ENCODE_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


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


/**
 * @brief Encoder object to manage data to encode
 */
typedef struct
{
    uint8_t *buffer;
    uint8_t len;
    uint8_t index;
    bool overflow;
} encoder_t;

/**
 * @brief Decode object to manage data to decode
 */
typedef struct
{
    const uint8_t *buffer;
    uint8_t len;
    uint8_t index;
    bool overflow;
} decoder_t;

/**
 * @brief Encodes bytes into buffer
 * 
 * @param encoder pointer to encoder object
 * @param data data to encode
 * @param len length of data to encode
 */
void encoder_encode_bytes(encoder_t *encoder, const uint8_t *data, uint8_t len);

/**
 * @brief Decodes buffer into bytes
 * 
 * @param decoder pointer to decode object
 * @param data pointer to store decoded data
 * @param len length of data to decode
 */
void encoder_decode_bytes(decoder_t *decoder, uint8_t *data, uint8_t len);

/**
 * @brief Decodes remaining bytes in buffer
 * 
 * @param decoder pointer to decode object
 * @param data pointer to store decoded data
 * @param len length of data that was decoded
 */
void encoder_decode_remaining(decoder_t *decoder, uint8_t *data, uint8_t *len);

#endif
