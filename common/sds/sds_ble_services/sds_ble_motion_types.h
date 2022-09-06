#ifndef SDS_BLE_MOTION_TYPES_H_
#define SDS_BLE_MOTION_TYPES_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "sdk_common.h"
#include "sds_error.h"

/**
	@brief	Preambles for the three packet types
 */
typedef uint8_t sds_preamble_t;
#define MOTION_SAMPLE_PREAMBLE      0xAA
#define COMMAND_PREAMBLE            0xBB 
#define RESPONSE_PREAMBLE           0xCC


/**
	@brief	All possible commands that can be sent via the command structure. Some Require Args. See Main Application command handler
 */
typedef enum
{
	BASE_OFFSET = 0x20,
    START_SESSION = BASE_OFFSET,
    STOP_SESSION,
    START_STREAM,
    STOP_STREAM,	
	GET_SAMPLE_RATE,
	SET_SAMPLE_RATE,
	RUN_MOTION_CAL,
	MIN_OPCODE_VAL = START_SESSION,
	MAX_OPCODE_VAL = RUN_MOTION_CAL,
} sds_opcode_t;

/**@brief Enumeration for session data destination for command. */
typedef enum
{
    SESSION_IDLE,
	SESSION_TO_CENTRAL,    
	SESSION_TO_MEMORY,
	SESSION_TO_MEMORY_AND_CENTRAL,
	MIN_SESSION_VAL = SESSION_TO_CENTRAL,
	MAX_SESSION_VAL = SESSION_TO_MEMORY_AND_CENTRAL,	
} sds_session_destination_t;

/**@brief Sturcture to hold command packet data. */
typedef struct {
	const uint8_t		preamble;
	const sds_opcode_t 	opcode;
	const uint8_t		arg_len;	
	const uint8_t * 	p_args;
} sds_command_t;

/**@brief Sturcture to hold response packet data. */
typedef struct {
	uint8_t				preamble;
	sds_opcode_t		opcode;
	sds_return_t		err_code;	
	uint8_t				arg_len;
	uint8_t * 			p_args;
} sds_response_t;

/**@brief */
typedef struct {
    const uint8_t * p_data;
    uint8_t len;
} sds_notif_t;



#endif
