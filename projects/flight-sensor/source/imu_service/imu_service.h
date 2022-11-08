/**
 * @file    command_handler.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "session_manager.h"

/**
 * @brief 
 */
typedef struct imu_service_s imu_service_t;

/**
 * @brief 
 */
typedef void (*on_command_callback_t)(uint8_t *payload, uint8_t len, void *context);

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
 * @brief
 */
typedef struct
{
    command_e type;
    uint8_t command_token;
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
 */
struct imu_service_s
{
    status_e (*send_response)(imu_service_t *service, uint8_t *payload, uint8_t len, bool retry);
    status_e (*send_update)(imu_service_t *service, uint8_t *payload, uint8_t len, bool retry);
    void (*on_command)(imu_service_t *service, on_command_callback_t callback);
};


/**
 * @brief 
 * 
 * @param service 
 * @return status_e 
 */
status_e imu_service_initialize(imu_service_t *service);

/**
 * @brief
 * 
 * @param service 
 * @param sample 
 * @return status_e 
 */
status_e imu_service_send_sample(imu_service_t *service, void *sample);

/**
 * @brief
 * 
 * @param service 
 * @param current 
 * @param previous 
 * @return status_e 
 */
status_e imu_service_send_state_update(imu_service_t *service, session_state_e current, session_state_e previous);

#endif
