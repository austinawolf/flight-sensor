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

    imu_config_t config = 
    {
        .rate = control->rate,
        .flags = control->flags,
    };

    if (control->session_time)
    {
        ret_code_t ret = app_timer_start(control->timer, APP_TIMER_TICKS(control->session_time * 1000u), NULL);
        if (ret != NRF_SUCCESS)
        {
            LOG_ERROR("app_timer_start failed, %d", ret);
        }
    }

    imu_start(&config);
}

static void _sampling_on_exit(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;

    imu_stop();

    ret_code_t ret = app_timer_stop(control->timer);
    if (ret != NRF_SUCCESS)
    {
        LOG_ERROR("app_timer_stop failed, %d", ret);
    }
}

static void _playback_on_entry(void *context)
{

}

static void _playback_on_exit(void *context)
{
    
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