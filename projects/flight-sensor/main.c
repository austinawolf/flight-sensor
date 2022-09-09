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
#include "state_machine.h"
#include "imu.h"
#include "timestamp.h"
#include "ble_helper.h"
#include "bsp_btn_ble.h"
#include "app_timer.h"
#include "command_handler.h"


static status_e _get_status(void)
{
    return STATUS_OK;
}

static status_e _start_sampling(imu_sample_rate_e rate, uint8_t flags, uint8_t destination, uint8_t sampling_time)
{
    event_t event = 
    {
        .event = EVENT_START_SAMPLING,
        .start_sampling = 
        {
            .rate = rate,
            .flags = flags,
            .time = sampling_time,
        }
    };
    state_machine_process(&event);

    return STATUS_OK;
}

static status_e _stop_sampling(void)
{
    event_t event = 
    {
        .event = EVENT_STOP_SAMPLING,
    };
    state_machine_process(&event);

    return STATUS_OK;
}

static status_e _start_playback(void)
{
    event_t event = 
    {
        .event = EVENT_START_PLAYBACK,
    };
    state_machine_process(&event);

    return STATUS_OK;
}

static status_e _stop_playback(void)
{
    event_t event = 
    {
        .event = EVENT_STOP_PLAYBACK,
    };
    state_machine_process(&event);

    return STATUS_OK;
}

static status_e _calibrate(void)
{
    event_t event = 
    {
        .event = EVENT_CALIBRATE,
    };
    state_machine_process(&event);

    return STATUS_OK;
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_lesc_request_handler();
    APP_ERROR_CHECK(err_code);

    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

static void _imu_sample_callback(imu_sample_t *sample)
{
    LOG_INFO("New sample @ %d ms", sample->timestamp);

    ble_helper_sample_send(sample);
}

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

/**@brief Function for application main entry.
 */
int main(void)
{
    const command_handler_callbacks_t callbacks = 
    {
        .get_status = _get_status,
        .start_sampling = _start_sampling,
        .stop_sampling = _stop_sampling,
        .start_playback = _start_playback,
        .stop_playback = _stop_playback,
        .calibrate = _calibrate,
    };

    // Initialize.
    logger_create();
    initialize();
    ble_helper_create();
    state_machine_create();
    imu_create();
    timestamp_create();
    command_handler_register_callbacks(&callbacks);

    LOG_INFO("Flight Sensor Started.");

    imu_register_sample_callback(_imu_sample_callback);
    ble_helper_advertising_start(false);

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}


