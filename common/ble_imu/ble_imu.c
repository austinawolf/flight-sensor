#include "sdk_common.h"
#include "ble_imu.h"
#include <string.h>
#include "ble_srv_common.h"
#include "logger.h"


#define OPCODE_LENGTH 1                                                              /**< Length of opcode inside Heart Rate Measurement packet. */
#define HANDLE_LENGTH 2                                                              /**< Length of handle inside Heart Rate Measurement packet. */
#define MAX_HRM_LEN      (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum size of a transmitted Heart Rate Measurement. */


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_hrs_t * p_hrs, ble_evt_t const * p_ble_evt)
{
    p_hrs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_hrs_t * p_hrs, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_hrs->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling write events to the Heart Rate Measurement characteristic.
 *
 * @param[in]   p_hrs         Heart Rate Service structure.
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_hrm_cccd_write(ble_hrs_t * p_hrs, ble_gatts_evt_write_t const * p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        // CCCD written, update notification state
        if (p_hrs->evt_handler != NULL)
        {
            ble_hrs_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_HRS_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_HRS_EVT_NOTIFICATION_DISABLED;
            }

            p_hrs->evt_handler(p_hrs, &evt);
        }
    }
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_hrs_t * p_hrs, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == p_hrs->hrm_handles.cccd_handle)
    {
        on_hrm_cccd_write(p_hrs, p_evt_write);
    }
    
    LOG_INFO("On write.");
}


void ble_hrs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_hrs_t * p_hrs = (ble_hrs_t *) p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_hrs, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_hrs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_hrs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for encoding a Heart Rate Measurement.
 *
 * @param[in]   p_hrs              Heart Rate Service structure.
 * @param[in]   heart_rate         Measurement to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t hrm_encode(ble_hrs_t * p_hrs, uint16_t heart_rate, uint8_t * p_encoded_buffer)
{
    uint8_t flags = 0;
    uint8_t len   = 1;

    // Encode heart rate measurement
    len += uint16_encode(heart_rate, &p_encoded_buffer[len]);

    // Add flags
    p_encoded_buffer[0] = flags;

    return len;
}


uint32_t ble_hrs_init(ble_hrs_t * p_hrs, const ble_hrs_init_t * p_hrs_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;
    uint8_t               encoded_initial_hrm[MAX_HRM_LEN];

    // Initialize service structure
    p_hrs->evt_handler                 = p_hrs_init->evt_handler;
    p_hrs->conn_handle                 = BLE_CONN_HANDLE_INVALID;
    p_hrs->max_hrm_len                 = MAX_HRM_LEN;
    p_hrs->uuid_type                   = BLE_UUID_TYPE_VENDOR_BEGIN;

    // Add a custom base UUID.
    ble_uuid128_t base_uuid = {BLE_UUID_COMMAND_CHAR_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_hrs->uuid_type);
    APP_ERROR_CHECK(err_code);

    // Add service
    ble_uuid.type = p_hrs->uuid_type;
    ble_uuid.uuid = BLE_UUID_COMMAND_CHAR;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_hrs->service_handle);

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add heart rate measurement characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));

    add_char_params.uuid              = BLE_UUID_COMMAND_CHAR;
    add_char_params.max_len           = MAX_HRM_LEN;
    add_char_params.init_len          = 0;
    add_char_params.p_init_value      = encoded_initial_hrm;
    add_char_params.is_var_len        = true;
    add_char_params.char_props.write  = 1;
    add_char_params.write_access      = SEC_OPEN;
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = p_hrs_init->hrm_cccd_wr_sec;

    err_code = characteristic_add(p_hrs->service_handle, &add_char_params, &(p_hrs->hrm_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add heart rate measurement characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = BLE_UUID_DATA_CHAR;
    add_char_params.max_len           = 20;
    add_char_params.init_len          = 0;
    add_char_params.p_init_value      = encoded_initial_hrm;
    add_char_params.is_var_len        = true;
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = p_hrs_init->hrm_cccd_wr_sec;

    err_code = characteristic_add(p_hrs->service_handle, &add_char_params, &(p_hrs->bsl_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


uint32_t ble_hrs_heart_rate_measurement_send(ble_hrs_t * p_hrs, uint16_t heart_rate)
{
    uint32_t err_code;

    // Send value if connected and notifying
    if (p_hrs->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint8_t                encoded_hrm[MAX_HRM_LEN];
        uint16_t               len;
        uint16_t               hvx_len;
        ble_gatts_hvx_params_t hvx_params;

        len     = hrm_encode(p_hrs, heart_rate, encoded_hrm);
        hvx_len = len;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_hrs->hrm_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &hvx_len;
        hvx_params.p_data = encoded_hrm;

        err_code = sd_ble_gatts_hvx(p_hrs->conn_handle, &hvx_params);
        if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}

void ble_hrs_on_gatt_evt(ble_hrs_t * p_hrs, nrf_ble_gatt_evt_t const * p_gatt_evt)
{
    if (    (p_hrs->conn_handle == p_gatt_evt->conn_handle)
        &&  (p_gatt_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        p_hrs->max_hrm_len = p_gatt_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    }
}
