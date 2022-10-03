/**
 * @file    session_state_machine.h
 * @author  Austin Wolf
 * @brief
 */

#include "session_manager_internal.h"
#include "session_manager.h"
#include "state_machine.h"
#include "logger.h"
#include "imu.h"
#include "app_scheduler.h"


static const state_t _idle;
static const state_t _streaming;
static const state_t _recording;
static const state_t _playback;
static const state_t _calibrating;


static void _calibrate(void * p_event_data, uint16_t event_size)
{
    imu_calibrate();
    on_calibration_done(true);
}

static void _streaming_on_entry(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;

    if (control->session_time)
    {
        ret_code_t ret = app_timer_start(control->timer, APP_TIMER_TICKS(control->session_time * 1000u), NULL);
        if (ret != NRF_SUCCESS)
        {
            LOG_ERROR("app_timer_start failed, %d", ret);
        }
    }

    imu_config_t config = 
    {
        .rate = control->rate,
        .flags = control->flags,
    };

    imu_start(&config);
}

static void _streaming_on_exit(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;

    imu_stop();

    ret_code_t ret = app_timer_stop(control->timer);
    if (ret != NRF_SUCCESS)
    {
        LOG_ERROR("app_timer_stop failed, %d", ret);
    }
}

static void _recording_on_entry(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;

    status_e status = session_store_open();
    if (status != STATUS_OK)
    {
        LOG_ERROR("session_store_open failed, err: %d", status)
        return;
    }

    if (control->session_time)
    {
        ret_code_t ret = app_timer_start(control->timer, APP_TIMER_TICKS(control->session_time * 1000u), NULL);
        if (ret != NRF_SUCCESS)
        {
            LOG_ERROR("app_timer_start failed, %d", ret);
        }
    }

    imu_config_t config = 
    {
        .rate = control->rate,
        .flags = control->flags,
    };

    imu_start(&config);
}

static void _recording_on_exit(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;
    
    status_e status = session_store_close();
    if (status != STATUS_OK)
    {
        LOG_ERROR("session_store_close failed, err: %d", status)
        return;
    }

    imu_stop();

    ret_code_t ret = app_timer_stop(control->timer);
    if (ret != NRF_SUCCESS)
    {
        LOG_ERROR("app_timer_stop failed, %d", ret);
    }
}

static void _playback_on_entry(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;
    
    control->playback_index = 0u;
}

static void _playback_on_exit(void *context)
{
    
}

static void _calibrate_on_entry(void *context)
{
    app_sched_event_put(NULL, 0, _calibrate);
}

static const state_t _idle =
{
    .name = "IDLE",
    .id = SESSION_STATE_IDLE,
    .on_entry = NULL,
    .on_exit = NULL,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_STREAM, .next = &_streaming,     .name="Stream"},
        {.event = SESSION_EVENT_RECORD, .next = &_recording,     .name="Record"},
        {.event = SESSION_EVENT_PLAYBACK, .next = &_playback,     .name="Playback"},
        {.event = SESSION_EVENT_CALIBRATE,      .next = &_calibrating,  .name="Calibrate"},
        {.event = NULL_TRANSITION}
    }
};

static const state_t _streaming =
{
    .name = "STREAMING",
    .id = SESSION_STATE_STREAMING,
    .on_entry = _streaming_on_entry,
    .on_exit = _streaming_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_STOP,           .next = &_idle,     .name="Stop"},
        {.event = SESSION_EVENT_TIMEOUT,        .next = &_idle,     .name="Timeout"},
        {.event = SESSION_EVENT_FLASH_FULL,     .next = &_idle,     .name="Flash Full"},
        {.event = SESSION_EVENT_DISCONNECTED,   .next = &_idle,     .name="Disconnected"},
        {.event = NULL_TRANSITION}
    }
};

static const state_t _recording =
{
    .name = "RECORDING",
    .id = SESSION_STATE_RECORDING,
    .on_entry = _recording_on_entry,
    .on_exit = _recording_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_STOP,  .next = &_idle,     .name="Stop"},
        {.event = SESSION_EVENT_TIMEOUT,        .next = &_idle,     .name="Timeout"},
        {.event = SESSION_EVENT_FLASH_FULL,     .next = &_idle,     .name="Flash Full"},
        {.event = NULL_TRANSITION}
    }
};

static const state_t _playback =
{
    .name = "PLAYBACK",
    .id = SESSION_STATE_PLAYBACK,
    .on_entry = _playback_on_entry,
    .on_exit = _playback_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_STOP,           .next = &_idle,     .name="Stop"},
        {.event = SESSION_EVENT_PLAYBACK_DONE,  .next = &_idle,     .name="Done"},
        {.event = SESSION_EVENT_DISCONNECTED,   .next = &_idle,     .name="Disconnected"},
        {.event = NULL_TRANSITION}
    }
};

static const state_t _calibrating =
{
    .name = "CALIBRATING",
    .id = SESSION_STATE_CALIBRATING,
    .on_entry = _calibrate_on_entry,
    .on_exit = NULL,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_SUCCESS, .next = &_idle,    .name="Success"},
        {.event = SESSION_EVENT_ERROR, .next = &_idle,      .name="Failed"},
        {.event = NULL_TRANSITION}
    }
};


const state_t *session_initial_state = &_idle;