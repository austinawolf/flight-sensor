#include "session_manager_internal.h"
#include "session_manager.h"
#include "state_machine.h"
#include "logger.h"
#include "imu.h"

static const state_t _idle;
static const state_t _sampling;
static const state_t _playback;
static const state_t _calibrating;


static void _idle_on_entry(void *context)
{

}

static void _idle_on_exit(void *context)
{

}

static void _sampling_on_entry(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;

    if ((control->destination == SESSION_DESTINATION_MEMORY) || (control->destination == SESSION_DESTINATION_BOTH))
    {
        status_e status = session_store_open();
        if (status != STATUS_OK)
        {
            LOG_ERROR("session_store_open failed, err: %d", status)
            return;
        }
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

static void _sampling_on_exit(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;

    if ((control->destination == SESSION_DESTINATION_MEMORY) || (control->destination == SESSION_DESTINATION_BOTH))
    {
        status_e status = session_store_close();
        if (status != STATUS_OK)
        {
            LOG_ERROR("session_store_close failed, err: %d", status)
            return;
        }
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
    control->is_playback = true;
}

static void _playback_on_exit(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;
    
    control->is_playback = false;
}

static void _calibrate_on_entry(void *context)
{
    imu_calibrate();
    session_manager_on_calibration_done(true);
}

static void _calibrate_on_exit(void *context)
{
    
}

static const state_t _idle =
{
    .name = "IDLE",
    .on_entry = _idle_on_entry,
    .on_exit = _idle_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_START_SAMPLING, .next = &_sampling,     .name="Start Sampling"},
        {.event = SESSION_EVENT_START_PLAYBACK, .next = &_playback,     .name="Start Playback"},
        {.event = SESSION_EVENT_CALIBRATE,      .next = &_calibrating,  .name="Calibrate"},
        {.event = NULL_TRANSITION}
    }
};

static const state_t _sampling =
{
    .name = "SAMPLING",
    .on_entry = _sampling_on_entry,
    .on_exit = _sampling_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_STOP_SAMPLING,  .next = &_idle,     .name="Stop"},
        {.event = SESSION_EVENT_TIMEOUT,        .next = &_idle,     .name="Timeout"},
        {.event = SESSION_EVENT_FLASH_FULL,     .next = &_idle,  .name="Flash Full"},
        {.event = NULL_TRANSITION}
    }
};

static const state_t _playback =
{
    .name = "PLAYBACK",
    .on_entry = _playback_on_entry,
    .on_exit = _playback_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_STOP_PLAYBACK, .next = &_idle,      .name="Stop"},
        {.event = SESSION_EVENT_PLAYBACK_DONE, .next = &_idle,      .name="Done"},
        {.event = NULL_TRANSITION}
    }
};

static const state_t _calibrating =
{
    .name = "CALIBRATING",
    .on_entry = _calibrate_on_entry,
    .on_exit = _calibrate_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_SUCCESS, .next = &_idle,    .name="Success"},
        {.event = NULL_TRANSITION}
    }
};


const state_t *session_initial_state = &_idle;