/**
 * @file    command_handler.h
 * @author  Austin Wolf
 * @brief   High level interface to process commands and send IMU samples
 */

#ifndef IMU_SERVICE_H_
#define IMU_SERVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "session_manager.h"


/**
 * @brief Possible command opcodes
 */
typedef enum
{
    COMMAND_GET_STATUS = 0x20,
    COMMAND_STREAM,
    COMMAND_RECORD,
    COMMAND_PLAYBACK,
    COMMAND_STOP,
    COMMAND_CALIBRATE,
} command_type_e;

/**
 * @brief Possible notification types
 */
typedef enum
{
    NOTIFICATION_STATE_UPDATE = 1,
    NOTIFICATION_SAMPLE,
} notification_type_e;

/**
 * @brief Tagged union for representing all possible IMU commands
 */
typedef struct
{
    command_type_e type;
    union {
        struct {
            uint8_t rate;
            uint8_t flags;
            uint16_t sampling_time;
        } stream;   // params for stream command
        struct {
            uint8_t rate;
            uint8_t flags;
            uint16_t sampling_time;
        } record;   // params for record command
    };
} imu_command_t;

/**
 * @brief Tagged union for representing all possible IMU responses
 */
typedef struct
{
    command_type_e type;
    status_e status;
    union {
        struct {
            uint8_t x;  // TODO: Implement "get_status" command/response AW 12/17/22
        } get_status;
    };
} imu_response_t;

/**
 * @brief Tagged union for representing all possible IMU notifications
 */
typedef struct
{
    notification_type_e type;
    union {
        struct {
            uint8_t current;
            uint8_t previous;
        } state_update; // params for "state_update" notification
        struct {
             imu_sample_t sample;
        } sample;       // params for "sample" notification
        uint8_t *args;
    };
    uint8_t len;
} imu_notification_t;

/**
 * @brief Creates IMU service
 * 
 * @return status_e 
 */
status_e imu_service_create(void);

/**
 * @brief Sends an IMU sample
 * 
 * @param sample pointer to IMU sample
 *
 * @return STATUS_OK on success
 */
status_e imu_service_send_sample(imu_sample_t *sample);

/**
 * @brief Sends an session state update
 *
 * @param current current session state
 * @param previous previous session state
 *
 * @return STATUS_OK on success
 */
status_e imu_service_send_state_update(session_state_e current, session_state_e previous);

#endif
