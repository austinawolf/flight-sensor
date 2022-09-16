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


status_e session_manager_create(void);

status_e session_manager_get_status(void);

status_e session_manager_start_sampling(imu_sample_rate_e rate, uint8_t flags, session_destination_e destination, uint32_t session_time);

status_e session_manager_stop_sampling(void);

status_e session_manager_start_playback(void);

status_e session_manager_stop_playback(void);

status_e session_manager_calibrate(void);

status_e session_manager_on_calibration_done(bool success);

#endif
