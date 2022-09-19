/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include "session_manager.h"
#include "session_manager_internal.h"
#include "logger.h"
#include "imu.h"
#include "ble_helper.h"
#include "app_scheduler.h"


APP_TIMER_DEF(m_timeout_event_timer);

static session_manager_control_t _control = 
{
    .sm = {0},
    .rate = 0,
    .destination = 0,
    .flags = 0,
    .session_time = 0,
    .timer = m_timeout_event_timer,
};

static void _transition_callback(const state_t *new_state, const state_t *previous_state, const transition_t *transition)
{
    LOG_INFO("Session State Update: %s -> %s via %s", previous_state->name, new_state->name, transition->name);
}


static void _on_new_sample(imu_sample_t *sample)
{
    if ((_control.destination == SESSION_DESTINATION_CENTAL) || (_control.destination == SESSION_DESTINATION_BOTH))
    {
        status_e status = ble_helper_sample_send(sample);
        if (status != STATUS_OK)
        {
            LOG_ERROR("ble_helper_sample_send failed, err: %d", status);
        }
    }

    if ((_control.destination == SESSION_DESTINATION_MEMORY) || (_control.destination == SESSION_DESTINATION_BOTH))
    {
        // status_e status = sample_store_append(&_control.sample_store, sample, sizeof(imu_sample_t));
        // if (status != STATUS_OK)
        // {
        //     LOG_ERROR("sample_store_append failed, err: %d", status);
        // }
    }
}
static volatile uint8_t _samples_ready = 0u;

static void _imu_read_fifo_handler(void * p_event_data, uint16_t event_size)
{
    (void) p_event_data;
    (void) event_size;

    bool sample_ready = false;
    imu_sample_t sample = {0};
    do 
    {
        status_e status = imu_sample_read(&sample, &sample_ready);
        _samples_ready--;
        if (status != STATUS_OK)
        {
            LOG_ERROR("imu_sample_read failed, err: %d", status);
            break;
        }

        _on_new_sample(&sample);
        break;
    }
    while (_samples_ready);
}

static void _imu_sample_callback(void)
{

    _samples_ready++;
    if (_samples_ready >= 1)
    {
        app_sched_event_put(NULL, 0, _imu_read_fifo_handler);
    }
}

static void _process_event(void * p_event_data, uint16_t event_size)
{
    uint32_t event = *((uint32_t*) p_event_data);

    state_machine_on_event(&_control.sm, event);
}

static void _on_event(session_event_e event)
{
    uint32_t event_value = event;
    app_sched_event_put(&event_value, 4, _process_event);
}

static void _timer_timeout_handler(void * p_context)
{
    _on_event(SESSION_EVENT_TIMEOUT);
}

static void _transfer_samples_from_memory(void * p_event_data, uint16_t event_size)
{
    (void) p_event_data;
    (void) event_size;

    bool buffer_full = false;
    imu_sample_t sample = {0};

    do 
    {
        status_e status = STATUS_OK;

        // status = sample_store_read(NULL, 0, &sample, sizeof(imu_sample_t), 1);
        // if (status != STATUS_OK)
        // {
        //     LOG_ERROR("sample_store_read failed, err: %d", status);
        //     return;
        // }

        status = ble_helper_sample_send(&sample);
        if (status == STATUS_ERROR_BUFFER_FULL)
        {
            buffer_full = true;
        }
        else if(status != STATUS_OK)
        {
            LOG_ERROR("ble_helper_sample_send failed: %d", status);
            return;
        }
        break;
    }
    while (buffer_full);
}

static void _ble_helper_event_handler(ble_helper_event_e event)
{
    switch (event)
    {
        case BLE_HELPER_EVENT_DISCONNECTED:
        {
            if (_control.destination == SESSION_DESTINATION_CENTAL)
            {
                _on_event(SESSION_EVENT_STOP_SAMPLING);
            }
            break;
        }
        case BLE_HELPER_EVENT_NOTIF_TX_COMPLETE:
        {
            if (false)  // is playback state
            {
                app_sched_event_put(NULL, 0, _transfer_samples_from_memory);
            }
        }
        default:
            break;
    }
}

status_e session_manager_create(void)
{
    status_e status = state_machine_create(&_control.sm, session_initial_state, _transition_callback, &_control);
    if (status != STATUS_OK)
    {
        return status;
    }

    uint32_t err_code = app_timer_create(&m_timeout_event_timer,
                                        APP_TIMER_MODE_SINGLE_SHOT,
                                        _timer_timeout_handler);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    // status = sample_store_create(&_control.sample_store);
    // if (status != STATUS_OK)
    // {
    //     LOG_ERROR("sample_store_create failed, err: %d", status);
    //     return status;
    // }

    imu_register_callback(_imu_sample_callback);
    ble_helper_register_callback(_ble_helper_event_handler);

    return STATUS_OK;
}

status_e session_manager_get_status(void)
{

    return STATUS_OK;
}

status_e session_manager_start_sampling(imu_sample_rate_e rate, uint8_t flags, session_destination_e destination, uint32_t session_time)
{
    _control.rate = rate,
    _control.flags = flags,
    _control.destination = destination;
    _control.session_time = session_time;

    _on_event(SESSION_EVENT_START_SAMPLING);
    return STATUS_OK;
}

status_e session_manager_stop_sampling(void)
{
    _on_event(SESSION_EVENT_STOP_SAMPLING);
    return STATUS_OK;
}

status_e session_manager_start_playback(void)
{
    _on_event(SESSION_EVENT_START_PLAYBACK);
    return STATUS_OK;
}

status_e session_manager_stop_playback(void)
{
    _on_event(SESSION_EVENT_STOP_PLAYBACK);
    return STATUS_OK;
}

status_e session_manager_calibrate(void)
{
    _on_event(SESSION_EVENT_CALIBRATE);
    return STATUS_OK;
}

status_e session_manager_on_calibration_done(bool success)
{
    if (success)
    {
        _on_event(SESSION_EVENT_SUCCESS);
    }
    else
    {
        _on_event(SESSION_EVENT_ERROR);
    }

    return STATUS_OK;
}