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


status_e session_manager_create(void);

status_e session_manager_get_status(void);

status_e session_manager_start_sampling(imu_sample_rate_e rate, uint8_t flags, uint8_t destination, uint8_t sampling_time);

status_e session_manager_stop_sampling(void);

status_e session_manager_start_playback(void);

status_e session_manager_stop_playback(void);

status_e session_manager_calibrate(void);

#endif
