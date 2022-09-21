/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef SESSION_MANAGER_H_
#define SESSION_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu.h"


typedef enum
{
    SESSION_DESTINATION_CENTAL,
    SESSION_DESTINATION_MEMORY,
    SESSION_DESTINATION_BOTH,
} session_destination_e;

typedef enum
{
    SESSION_STATE_IDLE,
    SESSION_STATE_STREAMING,
    SESSION_STATE_RECORDING,
    SESSION_STATE_PLAYBACK,
    SESSION_STATE_CALIBRATING,
    SESSION_STATE_ERROR,
} session_state_e;


status_e session_manager_create(void);

status_e session_manager_get_status(void);

status_e session_manager_start_stream(imu_sample_rate_e rate, uint8_t flags, uint32_t session_time);

status_e session_manager_start_recording(imu_sample_rate_e rate, uint8_t flags, bool stream_enable, uint32_t session_time);

status_e session_manager_start_playback(void);

status_e session_manager_stop(void);

status_e session_manager_calibrate(void);

#endif
