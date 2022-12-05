/**
 * @file    command_handler.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef IMU_SERVICE_H_
#define IMU_SERVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "session_manager.h"


/**
 * @brief 
 */
typedef void (*on_command_callback_t)(uint8_t *payload, uint8_t len, uint8_t sequence, void *context);

/**
 * @brief Possible command opcodes
 */
typedef enum
{
    COMMAND_GET_STATUS,
    COMMAND_STREAM,
    COMMAND_RECORD,
    COMMAND_PLAYBACK,
    COMMAND_STOP,
    COMMAND_CALIBRATE,
} command_e;

/**
 * @brief Possible command opcodes
 */
typedef enum
{
    UPDATE_STATE_UPDATE,
    UPDATE_SAMPLE,
} update_e;

/**
 * @brief
 */
typedef struct
{
    command_e type;
    union {
        struct {
            uint8_t rate;
            uint8_t flags;
            uint16_t sampling_time;
        } stream;
        struct {
            uint8_t rate;
            uint8_t flags;
            uint16_t sampling_time;
        } record;
        uint8_t *args;
    };
    uint8_t len;
} command_t;

/**
 * @brief
 */
typedef struct
{
    command_e type;
    uint8_t command_token;
    status_e status;
    union {
        struct {
            uint8_t x;
        } get_status;
    };
    uint8_t len;
} response_t;


/**
 * @brief 
 * 
 * @param service 
 * @return status_e 
 */
status_e imu_service_create(void);

/**
 * @brief
 * 
 * @param service 
 * @param sample 
 * @return status_e 
 */
status_e imu_service_send_sample(imu_sample_t *sample);

/**
 * @brief
 * 
 * @param service 
 * @param current 
 * @param previous 
 * @return status_e 
 */
status_e imu_service_send_state_update(session_state_e current, session_state_e previous);

#endif
