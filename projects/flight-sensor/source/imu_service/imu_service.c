/**
 * @file    command_handler.c
 * @author  Austin Wolf
 * @brief
 */
#include "imu_service.h"
#include "imu_service_encode.h"
#include "logger.h"
#include "command_handler.h"


/**
 * @see imu_service.h
 */
static void _on_command(uint8_t *payload, uint8_t len, uint8_t sequence, void *context)
{
    status_e status = STATUS_OK;
    imu_service_t *service = context;

    // decode command
    command_t command = {0};
    status = imu_service_command_decode(payload, len, &command);
    if (status != STATUS_OK)
    {
        LOG_WARNING("imu_service_command_decode failed, err: %d", status);
        return;
    }

    LOG_INFO("Command: type=%d, sequence=%d", command.type, sequence);

    // call command handler
    response_t response = {0};
    (void) command_handler_process(&command, &response);

    // encode response
    uint8_t response_len = 0u;
    uint8_t response_payload[MAX_PAYLOAD_LEN] = {0};
    status = imu_service_response_encode(&response, response_payload, &response_len);
    if (status != STATUS_OK)
    {
        LOG_ERROR("imu_service_response_encode failed: %d", status);
        return;
    }

    // send response
    status = service->send_response(service, response_payload, response_len, sequence, true);
    if (status != STATUS_OK)
    {
        LOG_ERROR("service->send_response failed: %d", status);
        return;
    }
}

/**
 * @see imu_service.h
 */
status_e imu_service_initialize(imu_service_t *service)
{
    service->on_command(service, _on_command);

    return STATUS_OK;
}

/**
 * @see imu_service.h
 */
status_e imu_service_send_sample(imu_service_t *service, imu_sample_t *sample)
{
    status_e status;

    // encode update
    uint8_t len = 0u;
    uint8_t payload[MAX_PAYLOAD_LEN] = {0};
    status = imu_service_sample_encode(sample, payload, &len);
    if (status != STATUS_OK)
    {
        LOG_ERROR("imu_service_sample_encode failed: %d", status);
        return status;
    }

    // send update
    status = service->send_update(service, payload, len, true);
    if (status != STATUS_OK)
    {
        LOG_ERROR("service->send_response failed: %d", status);
        return status;
    }

    return STATUS_OK;
}

/**
 * @see imu_service.h
 */
status_e imu_service_send_state_update(imu_service_t *service, session_state_e current, session_state_e previous)
{
    status_e status;
    
    // encode update
    uint8_t len = 0u;
    uint8_t payload[MAX_PAYLOAD_LEN] = {0};
    status = imu_service_state_update_encode(current, previous, payload, &len);
    if (status != STATUS_OK)
    {
        LOG_ERROR("imu_service_state_update_encode failed: %d", status);
        return status;
    }

    // send update
    status = service->send_update(service, payload, len, true);
    if (status != STATUS_OK)
    {
        LOG_ERROR("service->send_response failed: %d", status);
        return status;
    }

    return STATUS_OK;
}
