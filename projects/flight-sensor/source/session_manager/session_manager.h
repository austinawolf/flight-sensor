/**
 * @file    session_manager.h
 * @author  Austin Wolf
 * @brief   High level control of IMU states, data collection and data flow
 */

#ifndef SESSION_MANAGER_H_
#define SESSION_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "imu.h"


/**
 * @brief Possible states of the session manager
 */
typedef enum
{
    SESSION_STATE_IDLE,
    SESSION_STATE_STREAMING,
    SESSION_STATE_RECORDING,
    SESSION_STATE_PLAYBACK,
    SESSION_STATE_CALIBRATING,
    SESSION_STATE_ERROR,
} session_state_e;


/**
 * @brief Initializes the session manager
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_manager_create(void);

/**
 * @brief Gets the current status of the session manager 
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_manager_get_status(void);

/**
 * @brief Starts streaming IMU sample data
 * 
 * @param rate Sample rate
 * @param flags Flags associated with sampling
 * @param session_time amount of time to stream, in seconds (0 if forever)
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_manager_start_stream(imu_sample_rate_e rate, uint8_t flags, uint32_t session_time);

/**
 * @brief Starts recording IMU data to memory
 * 
 * @param rate Sample rate
 * @param flags Flags associated with sampling
 * @param stream_enable true to also stream, false otherwise
 * @param session_time amount of time to stream, in seconds (0 if forever)
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_manager_start_recording(imu_sample_rate_e rate, uint8_t flags, bool stream_enable, uint32_t session_time);

/**
 * @brief Starts sending that was previously recorded in memory
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_manager_start_playback(void);

/**
 * @brief Stop the current action and return to idle
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_manager_stop(void);

/**
 * @brief Start calibrating the IMU
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e session_manager_calibrate(void);

#endif
