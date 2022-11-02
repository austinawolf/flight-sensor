#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_pwr_mgmt.h"
#include "logger.h"
#include "nrf_ble_lesc.h"
#include "ble_helper.h"
#include "session_manager.h"
#include "imu.h"
#include "timestamp.h"
#include "ble_helper.h"
#include "bsp_btn_ble.h"
#include "app_timer.h"
#include "command_handler.h"
#include "app_scheduler.h"
#include "twi.h"
#include "session_store.h"
#include "flash.h"
#include "datastore.h"


#define MAX_APP_SCHEDULER_QUEUE_SIZE    (10u)


/**
 * @brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Handler for event from BLE helper
 */
static void _ble_helper_event_handler(ble_helper_event_e event)
{
    uint32_t err_code = 0u;

    switch (event)
    {
        case BLE_HELPER_EVENT_IDLE:
            sleep_mode_enter();
            break;
        case BLE_HELPER_EVENT_ADVERTISING:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_HELPER_EVENT_CONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            break;
        default:
            break;
    }
}

/**
 * @brief Initializes Nordic drivers/modules
 */
static void initialize(void)
{
    ret_code_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // power management
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    // BSP
    bsp_event_t startup_event;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    logger_create();
    initialize();
    APP_SCHED_INIT(4, MAX_APP_SCHEDULER_QUEUE_SIZE);
    flash_create();
    datastore_create();
    ble_helper_create();
    session_manager_create();
    twi_init();
    imu_create();
    timestamp_create();
    session_store_create();
    ble_helper_register_callback(_ble_helper_event_handler);
    LOG_INFO("Flight Sensor Started.");

    ble_helper_advertising_start(false);

    // Enter main loop.
    while (true)
    {
        app_sched_execute();
        if (NRF_LOG_PROCESS() == false)
        {
            nrf_pwr_mgmt_run();
        }
    }
}


