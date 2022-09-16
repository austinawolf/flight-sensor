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

    imu_register_callback(_imu_sample_callback);

    return STATUS_OK;
}

status_e session_manager_get_status(void)
{

    return STATUS_OK;
}

status_e session_manager_start_sampling(imu_sample_rate_e rate, uint8_t flags, uint8_t destination, uint8_t sampling_time)
{
    _control.rate = rate,
    _control.flags = flags,
    _control.destination = destination;
    _control.session_time = sampling_time;

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