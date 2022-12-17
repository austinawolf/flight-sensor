/**
 * @file    ble_imu.c
 * @author  Austin Wolf
 * @brief   BLE IMU service transport layer
 */

#include "sdk_common.h"
#include "ble_imu.h"
#include <string.h>
#include "ble_srv_common.h"
#include "logger.h"
#include "status.h"
#include "ble_imu_encode.h"


// BLE IMU Instance.
BLE_IMU_DEF(_ble_imu);

/**
 * @brief Send a packet as a notification
 */
static status_e _send_message(uint8_t *buffer, uint8_t len)
{
    uint16_t hvx_len = (uint16_t) len;
    ble_gatts_hvx_params_t hvx_params = 
    {
        .handle = _ble_imu.command_handles.value_handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset = 0,
        .p_len = &hvx_len,
        .p_data = (uint8_t *) buffer,
    };

    ret_code_t err_code = sd_ble_gatts_hvx(_ble_imu.conn_handle, &hvx_params);
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
 * @brief Save an encoded message to retry later
 */
static status_e _retry_later(uint8_t *buffer, uint8_t len)
{
    ble_imu_retry_t retry = {0};
    retry.len = len;
    memcpy(&retry.buffer, buffer, len);

    return queue_append(&_ble_imu.retry_queue, &retry);
}

/**
 * @brief Try to transmit a packet from the retry buffer 
 */
static void _retry(void)
{
    status_e status = STATUS_OK;
    ble_imu_retry_t retry = {0};

    while (queue_number_of_entries(&_ble_imu.retry_queue))
    {
        status = queue_inspect(&_ble_imu.retry_queue, 0, &retry);
        if (status != STATUS_OK)
        {
            break;
        }

        status = _send_message(retry.buffer, retry.len);
        if (status != STATUS_OK)
        {
            LOG_ERROR("Failed to retry, %d", status);
            break;
        }

        (void) queue_pop(&_ble_imu.retry_queue, 0, NULL);
    }
}

/**
 * @see ble_imu.h
 */
static status_e _send_response(uint8_t *payload, uint8_t len, uint8_t sequence, bool retry)
{
    status_e status = STATUS_OK;

    ble_imu_message_t message =
    {
        .type = BLE_IMU_MESSAGE_RESPONSE,
        .sequence = sequence,
        .payload = {0},
        .len = len
    };
    memcpy(message.payload, payload, len);

    uint8_t buffer[MAX_MESSAGE_LEN] = {0};
    uint8_t buffer_len = 0;
    status = ble_imu_encode_message(&message, buffer, &buffer_len);

    status = _send_message(buffer, buffer_len);
    if (retry && (status == STATUS_ERROR_BUFFER_FULL))
    {
        LOG_WARNING("Buffer full, will retry later");
        status = _retry_later(buffer, buffer_len);
    }

    return status;
}

/**
 * @brief Helper function to execute when data is written to one of the characteristics in the BLE IMU service
 */
static void _on_write(ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    if (p_evt_write->handle == _ble_imu.command_handles.value_handle)
    {
        ble_imu_message_t message = {0};
        ble_imu_decode_message(p_evt_write->data, p_evt_write->len, &message);

        switch (message.type)
        {
            case BLE_IMU_MESSAGE_COMMAND:
            {
                ble_imu_message_t response = {0};
                uint8_t response_len = 0u;

                if (NULL == _ble_imu.on_command)
                {
                    break;
                }

                _ble_imu.on_command(message.payload, message.len, response.payload, &response_len);

                if (response_len > 0)
                {
                    status_e status = _send_response(response.payload, response_len, message.sequence, true);
                    if (STATUS_OK != status)
                    {
                        LOG_ERROR("_send_response failed: %d", status);
                    }
                }
                break;
            }
            default:
                LOG_WARNING("Unexpected message: %d", message.type);
                break;
        }
    }
}

/**
 * @see ble_imu.h
 */
status_e ble_imu_create(void)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure
    _ble_imu.conn_handle    = BLE_CONN_HANDLE_INVALID;
    _ble_imu.uuid_type      = BLE_UUID_TYPE_VENDOR_BEGIN;
    _ble_imu.packet_index   = 0;

    // Setup retry queue
    status_e status = queue_create(&_ble_imu.retry_queue, &_ble_imu.retry_buffer, RETRY_QUEUE_LEN, sizeof(ble_imu_retry_t));
    if (status != STATUS_OK)
    {
        return status;
    }

    // Add a custom base UUID.
    ble_uuid128_t base_uuid = {BLE_UUID_COMMAND_CHAR_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &_ble_imu.uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    // Add service
    ble_uuid.type = _ble_imu.uuid_type;
    ble_uuid.uuid = BLE_UUID_COMMAND_CHAR;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &_ble_imu.service_handle);

    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    // Add Communication Characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));

    add_char_params.uuid              = BLE_UUID_COMMAND_CHAR;
    add_char_params.max_len           = MAX_MESSAGE_LEN;
    add_char_params.init_len          = 0;
    add_char_params.p_init_value      = NULL;
    add_char_params.is_var_len        = true;
    add_char_params.char_props.write  = 1;
    add_char_params.write_access      = SEC_OPEN;
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(_ble_imu.service_handle, &add_char_params, &(_ble_imu.command_handles));
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR_INTERNAL;
    }

    return STATUS_OK;
}

/**
 * @see ble_imu.h
 */
status_e ble_imu_send_notification(uint8_t *payload, uint8_t len, bool retry)
{
    status_e status = STATUS_OK;

    ble_imu_message_t message =
    {
        .type = BLE_IMU_MESSAGE_NOTIFICATION,
        .payload = {0},
        .len = len
    };
    memcpy(message.payload, payload, len);

    uint8_t buffer[MAX_MESSAGE_LEN] = {0};
    uint8_t buffer_len = 0;
    status = ble_imu_encode_message(&message, buffer, &buffer_len);

    status = _send_message(buffer, buffer_len);
    if (retry && (status == STATUS_ERROR_BUFFER_FULL))
    {
        LOG_WARNING("Buffer full, will retry later");
        status = _retry_later(buffer, buffer_len);
    }

    return status;
}

/**
 * @see ble_imu.h
 */
void ble_imu_on_command(ble_imu_command_callback_t callback, void *context)
{
    _ble_imu.on_command = callback;
    _ble_imu.on_command_context = context;
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
            p_imu->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            p_imu->conn_handle = BLE_CONN_HANDLE_INVALID;
            (void) queue_flush(&p_imu->retry_queue);
            break;
        case BLE_GATTS_EVT_WRITE:
            _on_write(p_ble_evt);
            break;
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            _retry();
            break;
        default:
            // No implementation needed.
            break;
    }
}
