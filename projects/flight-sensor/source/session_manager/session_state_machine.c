#include "session_manager_internal.h"
#include "state_machine.h"
#include "logger.h"
#include "imu.h"

static const state_t _idle;
static const state_t _sampling;
// static const state_t _playback;
// static const state_t _calibrating;


static void _idle_on_entry(void *context)
{
    LOG_INFO("On exit idle");
}

static void _idle_on_exit(void *context)
{
    LOG_INFO("On entry idle");
}

static void _sampling_on_entry(void *context)
{
    session_manager_control_t *control = (session_manager_control_t*) context;

    imu_config_t config = 
    {
        .rate = control->rate,
        .flags = control->flags,
    };

    imu_start(&config);
}

static void _sampling_on_exit(void *context)
{
    imu_stop();
}

static const state_t _idle =
{
    .name = "IDLE",
    .on_entry = _idle_on_entry,
    .on_exit = _idle_on_exit,
    .on_reentry = NULL,
    .transitions = (transition_t[])
    {
        {.event = SESSION_EVENT_START_SAMPLING, .next = &_sampling},
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
        {.event = SESSION_EVENT_STOP_SAMPLING, .next = &_idle},
        {.event = NULL_TRANSITION}
    }
};

const state_t *session_initial_state = &_idle;