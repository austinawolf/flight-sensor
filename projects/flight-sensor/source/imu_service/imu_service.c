/**
 * @file    imu_service.c
 * @author  Austin Wolf
 * @brief   High level interface to process commands and send IMU samples
 */
#include "imu_service.h"
#include "imu_service_encode.h"
#include "logger.h"
#include "command_handler.h"
#include "ble_imu.h"


/**
 * Callback to handle commands received by the BLE IMU service
 */
static void _on_command(uint8_t *command_payload, uint8_t command_len, uint8_t *response_payload, uint8_t *response_len)
{
    status_e status = STATUS_OK;

    // decode command
    imu_command_t command = {0};
    status = imu_service_command_decode(command_payload, command_len, &command);
    if (status != STATUS_OK)
    {
        LOG_WARNING("imu_service_command_decode failed, err: %d", status);
        return;
    }

    // call command handler
    imu_response_t response = {0};
    (void) command_handler_process(&command, &response);

    // encode response
    status = imu_service_response_encode(&response, response_payload, response_len);
    if (status != STATUS_OK)
    {
        LOG_ERROR("imu_service_response_encode failed: %d", status);
        return;
    }
}

/**
 * @see imu_service.h
 */
status_e imu_service_create(void)
{
    ble_imu_on_command(_on_command, NULL);

    return STATUS_OK;
}

/**
 * @see imu_service.h
 */
status_e imu_service_send_sample(imu_sample_t *sample)
{
    status_e status;

    imu_notification_t notif = 
    {
        .type = NOTIFICATION_SAMPLE,
        .sample.sample = {0},
    };
    memcpy(&notif.sample.sample, sample, sizeof(imu_sample_t));

    // encode update
    uint8_t len = 0u;
    uint8_t payload[MAX_PAYLOAD_LEN] = {0};
    status = imu_service_notification_encode(&notif, payload, &len);
    if (status != STATUS_OK)
    {
        LOG_ERROR("imu_service_state_notification_encode failed: %d", status);
        return status;
    }

    // send update
    status = ble_imu_send_notification(payload, len, false);
    if (status != STATUS_OK)
    {
        return status;
    }

    return STATUS_OK;
}

/**
 * @see imu_service.h
 */
status_e imu_service_send_state_update(session_state_e current, session_state_e previous)
{
    status_e status;
    
    imu_notification_t notif = 
    {
        .type = NOTIFICATION_STATE_UPDATE,
        .state_update = 
        {
            .current = current,
            .previous = previous,
        },
    };

    // encode update
    uint8_t len = 0u;
    uint8_t payload[MAX_PAYLOAD_LEN] = {0};
    status = imu_service_notification_encode(&notif, payload, &len);
    if (status != STATUS_OK)
    {
        LOG_ERROR("imu_service_state_notification_encode failed: %d", status);
        return status;
    }

    // send update
    status = ble_imu_send_notification(payload, len, true); 
    if (status != STATUS_OK)
    {
        LOG_ERROR("ble_imu_send_update failed: %d", status);
        return status;
    }

    return STATUS_OK;
}
