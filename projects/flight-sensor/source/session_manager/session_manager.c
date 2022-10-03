/**
 * @file    session_manager.c
 * @author  Austin Wolf
 * @brief
 */
#include "session_manager.h"
#include "session_manager_internal.h"
#include "logger.h"
#include "imu.h"
#include "ble_helper.h"
#include "app_scheduler.h"

/* Definition of timeout timer */
APP_TIMER_DEF(m_timeout_event_timer);

/**
 * @brief Control structure used the session manager and state machine
 */
static session_manager_control_t _control = 
{
    .sm = {0},
    .rate = 0,
    .flags = 0,
    .session_time = 0,
    .timer = m_timeout_event_timer,
    .stream_enabled = false,
};

/**
 * @brief Callbackt to log if the session state has changed
 */
static void _transition_callback(const state_t *new_state, const state_t *previous_state, const transition_t *transition)
{
    LOG_INFO("Session State Update: %s -> %s via %s", previous_state->name, new_state->name, transition->name);

    status_e status = ble_helper_send_state_update(new_state->id, previous_state->id);
    if (status != STATUS_OK)
    {
        LOG_ERROR("ble_helper_send_state_update failed, err: %d", status);
    }
}

/**
 * @brief Gets the current state from the control block
 */
static session_state_e _get_current_state(void)
{
    return (session_state_e) _control.sm.current->id;
}

/**
 * @brief Scheduler callback to process event by session state machine 
 */
static void _process_event(void * p_event_data, uint16_t event_size)
{
    uint32_t event = *((uint32_t*) p_event_data);

    state_machine_on_event(&_control.sm, event);
}

/**
 * @brief Helper function to append an event to the scheduler to be processed later
 */
static void _add_event(session_event_e event)
{
    uint32_t event_value = event;
    app_sched_event_put(&event_value, 4, _process_event);
}

/**
 * @brief Scheduler callback to read a new sample from the IMU and stream/record
 * depending on configuration
 */
static void _imu_read_fifo_handler(void * p_event_data, uint16_t event_size)
{
    (void) p_event_data;
    (void) event_size;

    bool sample_ready = false;
    imu_sample_t sample = {0};
    status_e status = imu_sample_read(&sample, &sample_ready);
    if (status != STATUS_OK)
    {
        LOG_ERROR("imu_sample_read failed, err: %d", status);
        return;
    }

    if (_control.stream_enabled)
    {
        status = ble_helper_sample_send(&sample);
        if (status != STATUS_OK)
        {
            LOG_ERROR("ble_helper_sample_send failed, err: %d", status);
        }
    }

    if (_get_current_state() == SESSION_STATE_RECORDING)
    {
        status = session_store_append(&sample);
        if (status == STATUS_ERROR_FLASH_FULL)
        {
            _add_event(SESSION_EVENT_FLASH_FULL);
        }
        else if (status != STATUS_OK)
        {
            LOG_ERROR("session_store_append failed, err: %d", status);
        }
    }
}

/**
 * @brief Handler executed when a new IMU sample is ready
 */
static void _imu_sample_callback(void)
{
    app_sched_event_put(NULL, 0, _imu_read_fifo_handler);
}

/**
 * @brief Timeout handler for session manager timeout handler
 */
static void _timer_timeout_handler(void * p_context)
{
    _add_event(SESSION_EVENT_TIMEOUT);
}

/**
 * @brief Function will read and transmit samples from memory until transmit buffer is full
 */
static void _transfer_samples_from_memory(void * p_event_data, uint16_t event_size)
{
    (void) p_event_data;
    (void) event_size;

    bool buffer_full = false;
    imu_sample_t sample = {0};

    do 
    {
        status_e status = STATUS_OK;

        status = session_store_read(_control.playback_index, &sample);
        if (status == STATUS_ERROR_INVALID_PARAM)
        {
            _add_event(SESSION_EVENT_PLAYBACK_DONE);
            return;
        }
        else if (status != STATUS_OK)
        {
            LOG_ERROR("session_store_read failed, err: %d", status);
            return;
        }

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
        else
        {
            _control.playback_index++;
        }
    }
    while (!buffer_full);
}

/**
 * @brief Event handler for events generated by the BLE helper module
 */
static void _ble_helper_event_handler(ble_helper_event_e event)
{
    switch (event)
    {
        case BLE_HELPER_EVENT_DISCONNECTED:
        {
            _add_event(SESSION_EVENT_DISCONNECTED);
            break;
        }
        case BLE_HELPER_EVENT_NOTIF_TX_COMPLETE:
        {
            if (_get_current_state() == SESSION_STATE_PLAYBACK)
            {
                app_sched_event_put(NULL, 0, _transfer_samples_from_memory);
            }
            break;
        }
        default:
            break;
    }
}

/**
 * @see session_manager_internal.h
 */
void on_calibration_done(bool success)
{
    _add_event(success ? SESSION_EVENT_SUCCESS : SESSION_EVENT_ERROR);
}

/**
 * @see session_manager.h
 */
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
    ble_helper_register_callback(_ble_helper_event_handler);

    return STATUS_OK;
}

/**
 * @see session_manager.h
 */
status_e session_manager_get_status(void)
{

    return STATUS_OK;
}

/**
 * @see session_manager.h
 */
status_e session_manager_start_stream(imu_sample_rate_e rate, uint8_t flags, uint32_t session_time)
{
    _control.rate = rate,
    _control.flags = flags,
    _control.session_time = session_time;
    _control.stream_enabled = true;

    _add_event(SESSION_EVENT_STREAM);

    return STATUS_OK;
}

/**
 * @see session_manager.h
 */
status_e session_manager_start_recording(imu_sample_rate_e rate, uint8_t flags, bool stream_enable, uint32_t session_time)
{
    _control.rate = rate,
    _control.flags = flags,
    _control.stream_enabled = stream_enable;
    _control.session_time = session_time;

    _add_event(SESSION_EVENT_RECORD);

    return STATUS_OK;
}

/**
 * @see session_manager.h
 */
status_e session_manager_start_playback(void)
{
    _add_event(SESSION_EVENT_PLAYBACK);
    return STATUS_OK;
}

/**
 * @see session_manager.h
 */
status_e session_manager_stop(void)
{
    _control.stream_enabled = false;

    _add_event(SESSION_EVENT_STOP);
    return STATUS_OK;
}

/**
 * @see session_manager.h
 */
status_e session_manager_calibrate(void)
{
    _add_event(SESSION_EVENT_CALIBRATE);
    return STATUS_OK;
}
