#include "sdk_common.h"
#include "ble_imu.h"
#include <string.h>
#include "ble_srv_common.h"
#include "logger.h"
#include "imu_command_handler.h"
#include "status.h"


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_imu       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
    p_imu->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_imu       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_imu->conn_handle = BLE_CONN_HANDLE_INVALID;
}


static uint32_t _send_command_response(ble_imu_t * p_imu, imu_response_t *response, uint16_t len)
{
    uint32_t err_code;

    // Send value if connected and notifying
    if (p_imu->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    uint16_t hvx_len = len;
    ble_gatts_hvx_params_t hvx_params = {
        .handle = p_imu->command_handles.value_handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset = 0,
        .p_len = &hvx_len,
        .p_data = (uint8_t *) response,
    };

    err_code = sd_ble_gatts_hvx(p_imu->conn_handle, &hvx_params);
    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }

    return err_code;
}

/**@brief Function for handling write events to the Heart Rate Measurement characteristic.
 *
 * @param[in]   p_imu         Heart Rate Service structure.
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_command_cccd_write(ble_imu_t * p_imu, ble_gatts_evt_write_t const * p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        // CCCD written, update notification state
        if (p_imu->evt_handler != NULL)
        {
            ble_imu_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_IMU_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_IMU_EVT_NOTIFICATION_DISABLED;
            }

            p_imu->evt_handler(p_imu, &evt);
        }
    }
}

/**
 * @brief 
 * 
 * @param p_imu 
 * @param p_evt_write 
 */
static void on_command_value_write(ble_imu_t * p_imu, ble_gatts_evt_write_t const * p_evt_write)
{
    imu_command_t *command = (imu_command_t *) p_evt_write->data;
    imu_response_t response = {0};
    uint16_t response_len = 0u;

    imu_command_handler_process(command, 
                                p_evt_write->len, 
                                &response, 
                                &response_len);
    if (response_len != 0)
    {
        _send_command_response(p_imu, &response, response_len);
    }
}



/**@brief Function for handling the Write event.
 *
 * @param[in]   p_imu       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == p_imu->command_handles.cccd_handle)
    {
        on_command_cccd_write(p_imu, p_evt_write);
    }
    else if (p_evt_write->handle == p_imu->command_handles.cccd_handle)
    {
        //on_data_cccd_write(p_imu, p_evt_write);
    }
    else if (p_evt_write->handle == p_imu->command_handles.value_handle)
    {
		on_command_value_write(p_imu, p_evt_write);
    }
    
    LOG_INFO("On write.");
}


void ble_imu_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_imu_t * p_imu = (ble_imu_t *) p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_imu, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_imu, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_imu, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_imu_init(ble_imu_t * p_imu, const ble_imu_init_t * p_imu_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure
    p_imu->evt_handler                 = p_imu_init->evt_handler;
    p_imu->conn_handle                 = BLE_CONN_HANDLE_INVALID;
    p_imu->uuid_type                   = BLE_UUID_TYPE_VENDOR_BEGIN;

    // Add a custom base UUID.
    ble_uuid128_t base_uuid = {BLE_UUID_COMMAND_CHAR_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_imu->uuid_type);
    APP_ERROR_CHECK(err_code);

    // Add service
    ble_uuid.type = p_imu->uuid_type;
    ble_uuid.uuid = BLE_UUID_COMMAND_CHAR;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_imu->service_handle);

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add heart rate measurement characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));

    add_char_params.uuid              = BLE_UUID_COMMAND_CHAR;
    add_char_params.max_len           = 20;
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
        return err_code;
    }

    // Add heart rate measurement characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = BLE_UUID_DATA_CHAR;
    add_char_params.max_len           = 20;
    add_char_params.init_len          = 0;
    add_char_params.p_init_value      = NULL;
    add_char_params.is_var_len        = true;
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_imu->service_handle, &add_char_params, &(p_imu->data_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


uint32_t ble_imu_sample_send(ble_imu_t * p_imu, uint16_t heart_rate)
{
    uint32_t err_code;

    // Send value if connected and notifying
    if (p_imu->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    uint8_t                encoded_hrm[20];
    uint16_t               len;
    uint16_t               hvx_len;
    ble_gatts_hvx_params_t hvx_params;

    len = 5;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_len = len;
    hvx_params.handle = p_imu->data_handles.value_handle;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len  = &hvx_len;
    hvx_params.p_data = encoded_hrm;

    err_code = sd_ble_gatts_hvx(p_imu->conn_handle, &hvx_params);
    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }

    return err_code;
}

void ble_imu_on_gatt_evt(ble_imu_t * p_imu, nrf_ble_gatt_evt_t const * p_gatt_evt)
{
    if ((p_imu->conn_handle == p_gatt_evt->conn_handle) &&  (p_gatt_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        //p_imu->max_hrm_len = p_gatt_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    }
}
