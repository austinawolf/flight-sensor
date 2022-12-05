/**
 * @file    ble_helper.c
 * @author  Austin Wolf
 * @brief   Module to initialize BLE stack and manage BLE services
 */

#include <stdint.h>
#include <string.h>
#include "ble_helper.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_imu.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "fds.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_lesc.h"
#include "nrf_ble_qwr.h"
#include "ble_conn_state.h"
#include "nrf_pwr_mgmt.h"
#include "logger.h"
#include "imu_service.h"


#define DEVICE_NAME                         "Flight Sensor"                         /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                   "Austin Wolf"                           /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                    300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION                    60000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define APP_BLE_CONN_CFG_TAG                1                                       /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO               3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(10, UNIT_1_25_MS)         /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(10, UNIT_1_25_MS)         /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY                       0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                       /**< Number of attempts before giving up the connection parameter negotiation. */
#define LESC_DEBUG_MODE                     0                                       /**< Set to 1 to use LESC debug keys, allows you to use a sniffer to inspect traffic. */
#define SEC_PARAM_BOND                      1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                      0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                      1                                       /**< LE Secure Connections enabled. */
#define SEC_PARAM_KEYPRESS                  0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES           BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                       0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE              7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE              16                                      /**< Maximum encryption key size. */
#define DEAD_BEEF                           0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define MAX_EVENT_HANDLERS                  (4u)                                    /**< Max number of BLE Helper event handlers that can be registered */


/**< GATT module instance. */
NRF_BLE_GATT_DEF(m_gatt); 

/**< Context for the Queued Write module.*/
NRF_BLE_QWR_DEF(m_qwr);       

/**< Advertising module instance. */
BLE_ADVERTISING_DEF(m_advertising);

/**< Handle of the current connection. */
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;    

/**< List of event handlers to execute when BLE helper event has occured*/
static uint8_t _event_handler_count = 0u;
static ble_helper_event_handler_t _event_handlers[MAX_EVENT_HANDLERS] = {0};

/**< Universally unique service identifiers. */
static ble_uuid_t m_adv_uuids[] =                                   
{
    {BLE_UUID_IMU_SERVICE,           BLE_UUID_TYPE_BLE},
};


/**
 * @brief Helper function to execute all registered event handlers
 */
static void _on_event(ble_helper_event_e event)
{
    for (uint8_t i = 0; i < _event_handler_count; i++)
    {
        _event_handlers[i](event);
    }
}

/**
 * @brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Clear bond information from persistent storage.
 */
static void _delete_bonds(void)
{
    ret_code_t err_code;

    LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void _pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            ble_helper_advertising_start(false);
            break;

        default:
            break;
    }
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void _gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief GATT module event handler.
 */
static void _gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
    {
        LOG_INFO("GATT ATT MTU on connection 0x%x changed to %d.",
                     p_evt->conn_handle,
                     p_evt->params.att_mtu_effective);
    }
}

/**@brief Function for initializing the GATT module.
 */
static void _gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, _gatt_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void _nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void _services_init(void)
{
    ret_code_t         err_code = {0};
    ble_dis_init_t     dis_init = {0};
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = _nrf_qwr_error_handler;
    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);
    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *  @note   All this function does is to disconnect. This could have been done by simply
 *          setting the disconnect_on_fail config parameter, but instead we use the event
 *          handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void _on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**
 * @brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief Function for initializing the Connection Parameters module.
 */
static void _conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = _on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void _on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            LOG_INFO("Fast advertising.");
            _on_event(BLE_HELPER_EVENT_ADVERTISING);
            break;
        case BLE_ADV_EVT_IDLE:
            _on_event(BLE_HELPER_EVENT_IDLE);
            break;
        default:
            break;
    }
}

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void _ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            LOG_INFO("Connected.");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            if (false)
            {
                err_code = pm_conn_secure(p_ble_evt->evt.gap_evt.conn_handle, false);
                if (err_code != NRF_ERROR_BUSY)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }

            _on_event(BLE_HELPER_EVENT_CONNECTED);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            LOG_INFO("Disconnected, reason %d.",
                          p_ble_evt->evt.gap_evt.params.disconnected.reason);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            _on_event(BLE_HELPER_EVENT_DISCONNECTED);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;
    
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            LOG_DEBUG("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
            break;
        
        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            LOG_INFO("BLE_GAP_EVT_AUTH_KEY_REQUEST");
            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            LOG_INFO("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
            break;

         case BLE_GAP_EVT_AUTH_STATUS:
             LOG_INFO("BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
                          p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                          p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                          p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
            break;
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
        {
            _on_event(BLE_HELPER_EVENT_NOTIF_TX_COMPLETE);
            break;
        }
        default:
            // No implementation needed.
            break;
    }
}

/**
 * @brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void _ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, _ble_evt_handler, NULL);
}

/**
 * @brief Function for the Peer Manager initialization.
 */
static void _peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(_pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for initializing the Advertising functionality.
 */
static void _advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;

    init.evt_handler = _on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**
 * @see ble_helper.h
 */
status_e ble_helper_create(void)
{ 
    // Initialize.
    _ble_stack_init();
    _gap_params_init();
    _gatt_init();
    _services_init();
    _advertising_init();
    _conn_params_init();
    _peer_manager_init();
 
    return STATUS_OK;
}

/**
 * @see ble_helper.h
 */
status_e ble_helper_register_callback(ble_helper_event_handler_t event_handler)
{
    if (_event_handler_count >= MAX_EVENT_HANDLERS)
    {
        return STATUS_ERROR_BUFFER_FULL;
    }

    _event_handlers[_event_handler_count++] = event_handler;

    return STATUS_ERROR;
}

/**
 * @see ble_helper.h
 */
void ble_helper_advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        _delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else
    {
        ret_code_t err_code;

        err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
    }
}
