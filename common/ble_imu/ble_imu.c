/**
 * @file    ble_imu.c
 * @author  Austin Wolf
 * @brief
 */

#include "sdk_common.h"
#include "ble_imu.h"
#include <string.h>
#include "ble_srv_common.h"
#include "logger.h"
#include "command_handler.h"
#include "status.h"
#include "ble_imu_types.h"
#include "ble_imu_encoder.h"


/**
 * @see ble_imu.h
 */
status_e ble_imu_init(ble_imu_t * p_imu, const ble_imu_init_t * p_imu_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure
    p_imu->conn_handle                 = BLE_CONN_HANDLE_INVALID;
    p_imu->uuid_type                   = BLE_UUID_TYPE_VENDOR_BEGIN;
    p_imu->packet_index                = 0;

    // Setup retry queue
    status_e status = queue_create(&p_imu->retry_queue, p_imu->retry_buffer, RETRY_QUEUE_LEN, sizeof(packet_retry_t));
    if (status != STATUS_OK)
    {
        return status;
    }

    // Add a custom base UUID.
    ble_uuid128_t base_uuid = {BLE_UUID_COMMAND_CHAR_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_imu->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    // Add service
    ble_uuid.type = p_imu->uuid_type;
    ble_uuid.uuid = BLE_UUID_COMMAND_CHAR;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_imu->service_handle);

    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    // Add heart rate measurement characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));

    add_char_params.uuid              = BLE_UUID_COMMAND_CHAR;
    add_char_params.max_len           = sizeof(ble_imu_packet_t);
    add_char_params.init_len          = 0;
    add_char_params.p_init_value      = NULL;
    add_char_params.is_var_len        = true;
    add_char_params.char_props.write  = 1;
    add_char_params.write_access      = SEC_OPEN;
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_imu->service_handle, &add_char_params, &(p_imu->command_handles));
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    // Add heart rate measurement characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = BLE_UUID_DATA_CHAR;
    add_char_params.max_len           = sizeof(ble_imu_packet_t);
    add_char_params.init_len          = 0;
    add_char_params.p_init_value      = NULL;
    add_char_params.is_var_len        = false;
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_imu->service_handle, &add_char_params, &(p_imu->data_handles));
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    return STATUS_OK;
}

/**
 * @brief Send a packet as a notification
 */
static status_e _transmit_packet(ble_imu_t * p_imu, ble_imu_packet_t *packet, uint8_t len)
{
    uint16_t hvx_len = (uint16_t) len;
    ble_gatts_hvx_params_t hvx_params = 
    {
        .handle = packet->header.preamble == BLE_IMU_PREAMBLE_DATA ? p_imu->data_handles.value_handle : p_imu->command_handles.value_handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset = 0,
        .p_len = &hvx_len,
        .p_data = (uint8_t *) packet,
    };

    ret_code_t err_code = sd_ble_gatts_hvx(p_imu->conn_handle, &hvx_params);
    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
    {
        return STATUS_ERROR_DATA_SIZE;
    }
    else if (err_code == NRF_ERROR_RESOURCES)
    {
        return STATUS_ERROR_BUFFER_FULL;
    }
    else if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    return STATUS_OK;
}

/**
 * @brief Save a notification to retry later
 */
static status_e _retry_later(ble_imu_t *p_imu, ble_imu_packet_t *packet, uint8_t len)
{
    packet_retry_t retry = {0};
    retry.len = len;
    memcpy(&retry.packet, packet, sizeof(ble_imu_packet_t));

    return queue_append(&p_imu->retry_queue, &retry);
}

/**
 * @brief Try to transmit a packet from the retry buffer 
 */
static void _retry(ble_imu_t *p_imu)
{
    status_e status = STATUS_OK;
    packet_retry_t retry = {0};

    while (queue_number_of_entries(&p_imu->retry_queue))
    {
        status = queue_inspect(&p_imu->retry_queue, 0, &retry);
        if (status != STATUS_OK)
        {
            break;
        }

        status = _transmit_packet(p_imu, &retry.packet, retry.len);
        if (status != STATUS_OK)
        {
            LOG_ERROR("Failed to retry, %d", status);
            break;
        }

        (void) queue_pop(&p_imu->retry_queue, 0, NULL);
    }
}

/**
 * @see ble_imu.h
 */
status_e ble_imu_sample_send(ble_imu_t * p_imu, imu_sample_t *sample)
{
    // Send value if connected and notifying
    if (p_imu->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return STATUS_ERROR_INVALID_STATE;
    }

    ble_imu_packet_t packet = {0};
    uint8_t packet_len = 0;
    ble_imu_encode_data(sample, p_imu->packet_index, &packet, &packet_len);

    status_e status = _transmit_packet(p_imu, &packet, packet_len);
    if (status == STATUS_OK)
    {
        p_imu->packet_index++;
    }
    return status;
}

/**
 * @see ble_imu.h
 */
status_e ble_imu_send_state_update(ble_imu_t * p_imu, session_state_e current, session_state_e previous)
{
    if (p_imu->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return STATUS_ERROR_INVALID_STATE;
    }

    ble_imu_packet_t packet = {0};
    uint8_t packet_len = 0;
    ble_imu_encode_state_update(current, previous, &packet, &packet_len);

    status_e status = _transmit_packet(p_imu, &packet, packet_len);
    if (status == STATUS_ERROR_BUFFER_FULL)
    {
        LOG_WARNING("Buffer full. Will retry later.");
        status = _retry_later(p_imu, &packet, packet_len);
    }
    else if (status != STATUS_OK)
    {
        LOG_ERROR("_transmit_packet failed, err: %d", status);
    }
    return status;
}

/**
 * @brief Function called to decode process a new command and send a response
 */
static void _on_command_value_write(ble_imu_t * p_imu, ble_gatts_evt_write_t const * p_evt_write)
{
    // decode command
    ble_imu_packet_t *packet = (ble_imu_packet_t *) p_evt_write->data;
    command_payload_t command = {0};
    uint8_t payload_len = 0u;
    ble_imu_decode_command(packet, p_evt_write->len, &command, &payload_len);

    // process command
    response_payload_t response = {0};
    uint8_t response_len = 0;
    command_handler_process(&command, 
                            payload_len, 
                            &response, 
                            &response_len);
    
    // encode response
    ble_imu_packet_t response_packet = {0};
    uint8_t response_packet_len = 0u;
    ble_imu_encode_response(&response, response_len, &response_packet, &response_packet_len);

    // send response
    status_e status = _transmit_packet(p_imu, &response_packet, response_packet_len);
    if (status == STATUS_ERROR_BUFFER_FULL)
    {
        LOG_WARNING("Buffer full. Will retry later.");
        _retry_later(p_imu, &response_packet, response_packet_len);
    }
    else if (status != STATUS_OK)
    {
        LOG_ERROR("_transmit_packet failed, err: %d", status);
    }
}

/**
 * @brief Function for handling the Connect event.
 */
static void _on_connect(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
    p_imu->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**
 * @brief Function for handling the Disconnect event.
 */
static void _on_disconnect(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_imu->conn_handle = BLE_CONN_HANDLE_INVALID;

    (void) queue_flush(&p_imu->retry_queue);
}

/**
 * @brief Helper function to execute when data is written to one of the characteristics in the BLE IMU service
 */
static void _on_write(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    if (p_evt_write->handle == p_imu->command_handles.value_handle)
    {
		_on_command_value_write(p_imu, p_evt_write);
    }
}

/**
 * @see ble_imu.h
 */
void ble_imu_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_imu_t * p_imu = (ble_imu_t *) p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            _on_connect(p_imu, p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            _on_disconnect(p_imu, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
            _on_write(p_imu, p_ble_evt);
            break;
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            _retry(p_imu);
            break;
        default:
            // No implementation needed.
            break;
    }
}
