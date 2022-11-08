/**
 * @file    command_handler.c
 * @author  Austin Wolf
 * @brief
 */
#include "imu_service.h"


/**
 * @see imu_service.h
 */
static void _on_command(uint8_t *payload, uint8_t len, void *context)
{
    imu_service_t *service = context;

    // decode command

    // call command handler

    // encode response

    // send response
    (void) service;
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
status_e imu_service_send_sample(imu_service_t *service, void *sample)
{
    // encode update

    // send update

    return STATUS_OK;
}

/**
 * @see imu_service.h
 */
status_e imu_service_send_state_update(imu_service_t *service, session_state_e current, session_state_e previous)
{
    // encode update

    // send update

    return STATUS_OK;
}
