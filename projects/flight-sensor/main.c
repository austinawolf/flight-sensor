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
#include "app_scheduler.h"
#include "twi.h"


#define MAX_APP_SCHEDULER_QUEUE_SIZE    (10u)


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
    state_machine_add_event(&event);

    return STATUS_OK;
}

static status_e _stop_sampling(void)
{
    event_t event = 
    {
        .event = EVENT_STOP_SAMPLING,
    };
    state_machine_add_event(&event);

    return STATUS_OK;
}

static status_e _start_playback(void)
{
    event_t event = 
    {
        .event = EVENT_START_PLAYBACK,
    };
    state_machine_add_event(&event);

    return STATUS_OK;
}

static status_e _stop_playback(void)
{
    event_t event = 
    {
        .event = EVENT_STOP_PLAYBACK,
    };
    state_machine_add_event(&event);

    return STATUS_OK;
}

static status_e _calibrate(void)
{
    event_t event = 
    {
        .event = EVENT_CALIBRATE,
    };
    state_machine_add_event(&event);

    return STATUS_OK;
}

static void _imu_read_fifo_handler(void * p_event_data, uint16_t event_size)
{
    (void) p_event_data;
    (void) event_size;

    bool sample_ready = true;
    imu_sample_t sample = {0};
    do 
    {
        status_e status = imu_sample_read(&sample, &sample_ready);
        if (status != STATUS_OK)
        {
            LOG_ERROR("imu_sample_read failed, err: %d", status);
            break;
        }
        
        status = ble_helper_sample_send(&sample);
        if (status != STATUS_OK)
        {
            LOG_ERROR("ble_helper_sample_send failed, err: %d", status);
            break;
        }

        break;
    }
    while (sample_ready);
}

static void _imu_sample_callback(void)
{
    static volatile uint8_t _samples_ready = 0u;

    _samples_ready++;
    if (_samples_ready >= 1)
    {
        app_sched_event_put(NULL, 0, _imu_read_fifo_handler);
        _samples_ready = 0u;
    }
}

/**@brief Function for putting the chip into sleep mode.
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
        case BLE_HELPER_EVENT_DISCONNECTED:
        {
            event_t event = {.event = EVENT_DISCONNECTED};
            state_machine_add_event(&event);
            break;
        }
        default:
            break;
    }
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
    APP_SCHED_INIT(0, MAX_APP_SCHEDULER_QUEUE_SIZE);
    ble_helper_create(_ble_helper_event_handler);
    state_machine_create();
    twi_init();
    imu_create();
    timestamp_create();
    command_handler_register_callbacks(&callbacks);

    LOG_INFO("Flight Sensor Started.");

    imu_register_callback(_imu_sample_callback);
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


